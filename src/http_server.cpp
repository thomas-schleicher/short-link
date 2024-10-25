#include "../includes/http_server.hpp"
#include "http_connection.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/placeholders.hpp>
#include <sys/socket.h>

HttpServer::HttpServer(io_context& io_context, const ip::tcp::endpoint& endpoint)
    : io_context_(io_context), acceptor_(io_context, endpoint) {
    accept_connection();
}

void HttpServer::accept_connection() {
    HttpConnection::pointer new_connection = HttpConnection::create(io_context_);
    acceptor_.async_accept(new_connection->socket(), 
    bind(&HttpServer::handle_accept, this, new_connection, boost::asio::placeholders::error)
    );
}

void HttpServer::handle_accept(HttpConnection::pointer& new_connection, const boost::system::error_code& error_code) {
    if (!error_code) {
        new_connection->process_connection();
    }
    accept_connection();
}