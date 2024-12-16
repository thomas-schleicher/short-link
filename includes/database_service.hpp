#ifndef DATABASE_SERVICE_HPP
#define DATABASE_SERVICE_HPP

#include <boost/asio/io_context.hpp>
#include <boost/system/detail/error_code.hpp>
#include <cstddef>
#include <boost/redis/connection.hpp>
#include <functional>
#include <memory>
#include <mutex>

class DatabaseService {
public:
    static DatabaseService& getInstance(boost::asio::io_context& io_context, std::size_t pool_size);
    void asyncSetKey(const std::string& key, const std::string& value, std::function<void(boost::system::error_code)> callback);
    void asyncGetKey(const std::string& key, std::function<void(boost::system::error_code, std::string)> callback); //we could use a varient here for the result

private:    
    DatabaseService(boost::asio::io_context& io_context, std::size_t pool_size = 4);
    ~DatabaseService();
    DatabaseService(DatabaseService const&);
    void operator=(DatabaseService const&);

    std::shared_ptr<boost::redis::connection> getConnection();

    boost::asio::io_context& io_context_;
    std::size_t pool_size_;
    std::vector<std::shared_ptr<boost::redis::connection>> connection_pool_;
    std::size_t current_index_ = 0; //index used for round robin selection

    static DatabaseService* INSTANCE;
    static std::mutex singleton_mutex;
};

#endif