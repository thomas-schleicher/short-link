
#include "../includes/http_server.hpp"
#include <iostream>

using namespace boost::asio;

int main(int argc, char *argv[]) {
    const int port = 8080;

    std::cout << "Starting server!" << std::endl;
    try
    {
        io_context io_context;
        ip::tcp::endpoint endpoint(ip::tcp::v4(), port);
        HttpServer server(io_context, endpoint);
        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}