#include "../includes/http_connection.hpp"
#include "../includes/log_utils.hpp"
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/error.hpp>
#include <iostream>

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
                LOG(LogLevel::INFO, "Request details:");
                LOG(LogLevel::INFO, "\tMethod: " + std::string(self->request_.method_string()));
                LOG(LogLevel::INFO, "\tTarget: " + std::string(self->request_.target()));
                LOG(LogLevel::INFO, "\tBody: " + std::string(self->request_.body()));

                // Log before forwarding to RequestHandler
                LOG(LogLevel::INFO, "Forwarding request to RequestHandler...");
                self->response_ = self->request_handler_.handle(self->request_);

                self->write();
            } else {
                LOG(LogLevel::ERROR, "Error reading request: " + error_code.message());
            }
        });
}




void HttpConnection::write() {
    auto self = shared_from_this();
    std::visit(
        [this, self](auto& response) {
        http::async_write(socket_, response, 
        [self](boost::beast::error_code error_code, size_t bytes_transferred) {
            if (!error_code) {
                auto error_code_socket = self->socket_.shutdown(ip::tcp::socket::shutdown_send, error_code);
                if (error_code_socket) {
                    cerr << "Error shuting down socket: " << error_code_socket.message() << endl;
                }
            } else {
                cerr << "Error writing response: " << error_code.message() << endl;
                auto error_code_socket = self->socket_.shutdown(ip::tcp::socket::shutdown_both, error_code);
                if (error_code_socket) {
                    cerr << "Error shuting down socket: " << error_code_socket.message() << endl;
                }
            }
        });
        
    }, response_);
}