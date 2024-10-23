#ifndef TCP_CONNECTION_HPP
#define TCP_CONNECTION_HPP

#include <boost/system/detail/error_code.hpp>
#include <memory>
#include <boost/asio.hpp>

using namespace boost;
using namespace std;

class tcp_connection : public std::enable_shared_from_this<tcp_connection> {
public:
    typedef std::shared_ptr<tcp_connection> pointer;

    static pointer create(asio::io_context& io_context);
    asio::ip::tcp::socket& socket();
    void handle();

private:
    asio::ip::tcp::socket socket_;
    asio::streambuf buffer_;

    explicit tcp_connection(asio::io_context& io_context);

    void handle_read(const system::error_code& error, size_t bytes_transferred);
    void handle_write(const system::error_code& error, size_t bytes_transferred);
};

#endif