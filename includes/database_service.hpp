#ifndef DATABASE_SERVICE_HPP
#define DATABASE_SERVICE_HPP

#include <boost/asio/io_context.hpp>
#include <cstddef>
#include <boost/redis/connection.hpp>
#include <memory>

class DatabaseService {
public:
    static DatabaseService& getInstance(boost::asio::io_context& io_context, std::size_t pool_size);

    
private:
    std::shared_ptr<boost::redis::connection> getConnection(); //retrive a connection from the connection pool
    
    DatabaseService(boost::asio::io_context& io_context, std::size_t pool_size = 4);
    ~DatabaseService();
    DatabaseService(DatabaseService const&);
    void operator=(DatabaseService const&);

    boost::asio::io_context& io_context_;
    std::size_t pool_size_;
    std::vector<std::shared_ptr<boost::redis::connection>> connection_pool_;
    std::size_t current_index_ = 0; //index used for round robin selection
};

#endif