#include "../includes/http_connection.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/error.hpp>
#include <iostream>
#include <sys/socket.h>

HttpConnection::HttpConnection(io_context& io_context) 
: socket_(io_context) {};

HttpConnection::pointer HttpConnection::create(io_context& io_context) {
    return pointer(new HttpConnection(io_context));
}

ip::tcp::socket& HttpConnection::socket() {
    return socket_;
}

void HttpConnection::process_connection() {
    read();
}

void HttpConnection::read() {
    auto self = shared_from_this();
    http::async_read(socket_, buffer_, request_,
        [self](boost::beast::error_code error_code, size_t bytes_transferred) {
            if (!error_code) {
                cout << "Request received:\n" << self->request_ << endl;
                self->response_ = self->request_handler_.handle(self->request_);
                self->write();
            } else {
                cerr << "Error reading: " << error_code.message() << endl;
            }
        });
}

void HttpConnection::write() {
    auto self = shared_from_this();
    http::async_write(socket_, response_, 
        [self](boost::beast::error_code error_code, size_t bytes_transferred) {
            if (!error_code) {
                auto error_code_socket = self->socket_.shutdown(ip::tcp::socket::shutdown_send, error_code);
                if (error_code_socket) {
                    cerr << "Error shuting down socket: " << error_code_socket.message() << endl;
                }
            } else {
                cerr << "Error writing response: " << error_code.message() << endl;
            }
        });
}