#include "../includes/database_service.hpp"
#include "../includes/log_utils.hpp"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <stdexcept>
#include <mutex>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>

std::mutex DatabaseService::singleton_mutex;
DatabaseService* DatabaseService::INSTANCE = nullptr;

DatabaseService& DatabaseService::getInstance(const std::string& db_path, std::size_t pool_size) {

    std::lock_guard<std::mutex> lock(singleton_mutex);
    if (!INSTANCE) {
        INSTANCE = new DatabaseService(db_path, pool_size);
    }
    return *INSTANCE;
}

DatabaseService::DatabaseService(const std::string& db_path, std::size_t pool_size)
    : db_path_(db_path), pool_size_(pool_size), current_index_(0) {
    LOG(LogLevel::INFO, "Initializing DatabaseService");

    if (pool_size_ <= 0) {
        LOG(LogLevel::ERROR, "Invalid pool size");
        throw std::invalid_argument("Connection pool size must be greater than zero.");
    }

    for (std::size_t i = 0; i < pool_size_; ++i) {
        auto session = std::make_shared<soci::session>(soci::sqlite3, db_path_);
        connection_pool_.emplace_back(session);
    }
    LOG(LogLevel::INFO, "Connection Pool filled");
    try {
        auto conn = getConnection();
        *conn << "CREATE TABLE IF NOT EXISTS urls ("
                 "short_code CHAR(8) PRIMARY KEY, "
                 "original_url TEXT NOT NULL UNIQUE, "
                 "created_at INTEGER DEFAULT CURRENT_TIMESTAMP);";

        LOG(LogLevel::INFO, "Database schema initialized successfully");
    } catch (const std::exception& e) {
        LOG(LogLevel::ERROR, "Exception during schema creation: " + std::string(e.what()));
        throw;
    }
}

DatabaseService::~DatabaseService() {
    connection_pool_.clear();
}

std::shared_ptr<soci::session> DatabaseService::getConnection() {
    if (connection_pool_.empty()) {
        throw std::runtime_error("Connection pool is empty.");
    }

    LOG(LogLevel::INFO, "Returning connection at index: " + std::to_string(current_index_));
    auto conn = connection_pool_[current_index_];
    current_index_ = (current_index_ + 1) % connection_pool_.size();
    return conn;
}

std::string DatabaseService::generateShortUUID() {
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    std::string uuidStr = to_string(uuid);
    return uuidStr.substr(0, 8);
}

void DatabaseService::shortenURL(const std::string& longURL, std::function<void(std::error_code, std::string)> callback) {
    LOG(LogLevel::INFO, "Shortening URL: " + longURL);
    try {
        auto conn = getConnection();
        std::string shortCode;
        *conn << "SELECT short_code FROM urls WHERE original_url = :long_url", soci::use(longURL), soci::into(shortCode);

        if (!shortCode.empty()) {
            callback({}, shortCode);
            return;
        }

        shortCode = generateShortUUID();
        *conn << "INSERT INTO urls (short_code, original_url) VALUES (:short_code, :long_url)",
            soci::use(shortCode), soci::use(longURL);
        callback({}, shortCode);
    } catch (const std::exception& e) {
        LOG(LogLevel::ERROR, "Failed to shorten URL: " + std::string(e.what()));
        callback(std::make_error_code(std::errc::io_error), "");
    }
}

void DatabaseService::getLongURL(const std::string& shortCode, std::function<void(std::error_code, std::string)> callback) {
    LOG(LogLevel::INFO, "Getting long-URL from short-code: " + shortCode);
    try {
        auto conn = getConnection();
        std::string longURL;
        *conn << "SELECT original_url FROM urls WHERE short_code = :short_code", soci::use(shortCode), soci::into(longURL);

        if (!longURL.empty()) {
            callback({}, longURL);
        } else {
            LOG(LogLevel::ERROR, "Short code not found: " + shortCode);
            callback(std::make_error_code(std::errc::no_such_file_or_directory), "");
        }
    } catch (const std::exception& e) {
        LOG(LogLevel::ERROR, "Failed to retrieve long URL: " + std::string(e.what()));
        callback(std::make_error_code(std::errc::io_error), "");
    }
}