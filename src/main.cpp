#include "../includes/http_server.hpp"
#include "../includes/log_utils.hpp"
#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;

int main(int argc, char* argv[]) {
    const int port = 8080;

    LOG(LogLevel::INFO, "Starting Server on 127.0.0.1:" + std::to_string(port));

    try {
        io_context io_context;
        ip::tcp::endpoint endpoint(ip::tcp::v4(), port);

        HttpServer server(io_context, endpoint);

        io_context.run();
    } catch (const std::exception& e) {
        LOG(LogLevel::ERROR, "Exception occurred: " + std::string(e.what()));
    }

    return 0;
}
