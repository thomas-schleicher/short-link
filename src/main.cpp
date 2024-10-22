#include <boost/asio/io_context.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <exception>
#include <iostream>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

int main(int argc, char *argv[]) {
    try {
        net::io_context ioc;
        tcp::acceptor acceptor{ioc, {tcp::v4(), 8080}};

        std::cout << "Server is running on port 8080\n";

        while (true) {
            tcp::socket socket(ioc);
            acceptor.accept(socket);
            beast::flat_buffer buffer;

            http::request<http::string_body> req;
            http::read(socket, buffer, req);

            http::response<http::string_body> res{http::status::found, 11};

            res.set(http::field::location, "https://www.google.com");
            res.prepare_payload();

            http::write(socket, res);
            socket.shutdown(tcp::socket::shutdown_send);
        }
    } catch (std::exception e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}