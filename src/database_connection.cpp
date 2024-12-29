#include "../includes/database_service.hpp"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>
#include <mutex>
#include <stdexcept>

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
    if (pool_size_ <= 0) {
        throw std::invalid_argument("Connection pool size must be greater than zero.");
    }

    for (std::size_t i = 0; i < pool_size_; ++i) {
        auto session = std::make_shared<soci::session>(soci::sqlite3, db_path_);
        connection_pool_.emplace_back(session);
    }

    // Ensure the table exists
    auto conn = getConnection();
    *conn << "CREATE TABLE IF NOT EXISTS urls ("
             "short_code CHAR(8) PRIMARY KEY, "
             "original_url TEXT NOT NULL UNIQUE, "
             "created_at INTEGER DEFAULT CURRENT_TIMESTAMP);";
}

DatabaseService::~DatabaseService() {
    connection_pool_.clear();
}

std::shared_ptr<soci::session> DatabaseService::getConnection() {
    std::lock_guard<std::mutex> lock(singleton_mutex);
    if (connection_pool_.empty()) {
        throw std::runtime_error("Connection pool is empty.");
    }

    auto conn = connection_pool_[current_index_];
    current_index_ = (current_index_ + 1) % pool_size_;
    return conn;
}

std::string DatabaseService::generateShortUUID() {
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    std::string uuidStr = to_string(uuid);
    return uuidStr.substr(0, 8);
}

void DatabaseService::shortenURL(const std::string& longURL, std::function<void(std::error_code, std::string)> callback) {
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
        callback(std::make_error_code(std::errc::io_error), "");
    }
}

void DatabaseService::getLongURL(const std::string& shortCode, std::function<void(std::error_code, std::string)> callback) {
    try {
        auto conn = getConnection();
        std::string longURL;
        *conn << "SELECT original_url FROM urls WHERE short_code = :short_code", soci::use(shortCode), soci::into(longURL);

        if (!longURL.empty()) {
            callback({}, longURL);
        } else {
            callback(std::make_error_code(std::errc::no_such_file_or_directory), "");
        }
    } catch (const std::exception& e) {
        callback(std::make_error_code(std::errc::io_error), "");
    }
}