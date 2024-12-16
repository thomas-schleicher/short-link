#include "../includes/database_service.hpp"
#include <cstddef>


DatabaseService& DatabaseService::getInstance(boost::asio::io_context &io_context, std::size_t pool_size) {
    static DatabaseService INSTANCE;
    return INSTANCE;
}

DatabaseService::DatabaseService(boost::asio::io_context& io_context, std::size_t pool_size) : io_context_(io_context), pool_size_(pool_size) {

}

DatabaseService::~DatabaseService() {

}