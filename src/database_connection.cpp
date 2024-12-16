#include "../includes/database_service.hpp"
#include <boost/redis/connection.hpp>
#include <cstddef>
#include <memory>
#include <mutex>
#include <stdexcept>

std::mutex DatabaseService::singleton_mutex;
DatabaseService* DatabaseService::INSTANCE = nullptr;

DatabaseService& DatabaseService::getInstance(boost::asio::io_context &io_context, std::size_t pool_size) {
    std::lock_guard<std::mutex> lock(singleton_mutex);
    if (!INSTANCE) {
        INSTANCE = new DatabaseService(io_context, pool_size);
    }
    return *INSTANCE;
}

DatabaseService::DatabaseService(boost::asio::io_context& io_context, std::size_t pool_size) : io_context_(io_context), pool_size_(pool_size), current_index_(0) {
    if (pool_size_ <= 0) {
        throw std::invalid_argument("Connection pool size must be greater than zero.");
    }

    for (std::size_t i = 0; i < pool_size_; ++i) {
        auto conn = std::make_shared<boost::redis::connection>(io_context_);
        connection_pool_.emplace_back(conn);
    }
}

DatabaseService::~DatabaseService() {
    connection_pool_.clear();
}

std::shared_ptr<boost::redis::connection> DatabaseService::getConnection() {
    std::lock_guard<std::mutex> lock(singleton_mutex);
    if (connection_pool_.empty()) {
        throw std::runtime_error("Connection pool is empty.");
    }

    auto conn = connection_pool_[current_index_];
    current_index_ = (current_index_ + 1) % pool_size_;
    return conn; 
}