#include "../includes/http_server.hpp"
#include "../includes/log_utils.hpp"
#include <iostream>
#include "http_connection.hpp"
#include <boost/asio/placeholders.hpp>

HttpServer::HttpServer(io_context& io_context, const ip::tcp::endpoint& endpoint)
    : io_context_(io_context), acceptor_(io_context, endpoint) {
    accept_connection();
}

void HttpServer::accept_connection() {
    HttpConnection::pointer new_connection = HttpConnection::create(io_context_);

    LOG(LogLevel::INFO, "Waiting for new connection");
    acceptor_.async_accept(new_connection->socket(),
        bind(&HttpServer::handle_accept, this, new_connection, boost::asio::placeholders::error));
}

void HttpServer::handle_accept(HttpConnection::pointer& new_connection, const boost::system::error_code& error_code) {
    if (!error_code) {
        LOG(LogLevel::INFO, "Accepted new connection from: " + new_connection->socket().remote_endpoint().address().to_string());
        new_connection->process_connection();
    } else {
        LOG(LogLevel::ERROR, "Error accepting connection: " + error_code.message());
    }
    accept_connection();
}
