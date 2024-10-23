#include "../includes/tcp_connection.hpp"
#include <boost/asio/buffer.hpp>
#include <boost/asio/write.hpp>
#include <boost/system/detail/error_code.hpp>
#include <iostream>
#include <boost/bind/bind.hpp>

tcp_connection::tcp_connection(asio::io_context& io_context)
    : socket_(io_context) {}

tcp_connection::pointer tcp_connection::create(asio::io_context& io_context) {
    return pointer(new tcp_connection(io_context));
}

asio::ip::tcp::socket& tcp_connection::socket() {
    return socket_;
}

void tcp_connection::handle() {
    asio::async_read_until(socket_, buffer_, "\r\n",
        bind(&tcp_connection::handle_read, 
            shared_from_this(),
            asio::placeholders::error,
            asio::placeholders::bytes_transferred)
    );
}

void tcp_connection::handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
    if (!error) {
        // std::istream request_stream(&buffer_);
        // std::string request_line;
        // std::getline(request_stream, request_line);

        // auto parsed_data = request_parser_.parse(request_line);
        // string response = response_handler_.handle(parsed_data);

        std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, world!";
        auto data = asio::buffer(response);

        asio::async_write(socket_, data,
        bind(&tcp_connection::handle_write, 
            shared_from_this(),
            asio::placeholders::error,
            asio::placeholders::bytes_transferred)
        );
    } else {
        std::cerr << "Error during read: " << error.message() << std::endl;
    }
}

void tcp_connection::handle_write(const system::error_code& error, size_t bytes_transferred) {
    if (error) {
        std::cerr << "Error during write: " << error.message() << std::endl;
    } else {
        std::cout << "Successfully sent " << bytes_transferred << " bytes." << std::endl;
    }
}
