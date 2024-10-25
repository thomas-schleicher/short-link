#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include "http_connection.hpp"
#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/detail/error_code.hpp>

using namespace boost::asio;

class HttpServer {
public:
    HttpServer(io_context& io_context, const ip::tcp::endpoint& endpoint);

private:
    io_context& io_context_;
    ip::tcp::acceptor acceptor_;

    void accept_connection();
    void handle_accept(HttpConnection::pointer& new_connection, const boost::system::error_code& error_code);
};

#endif