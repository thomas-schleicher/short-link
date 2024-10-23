#include "../includes/tcp_connection.hpp"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>
#include <iostream>

class tcp_server {
    public:
        explicit tcp_server(boost::asio::io_context& io_context, const int port): io_context_(io_context), acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
            start_accept();
        }

    private:
        boost::asio::io_context& io_context_;
        boost::asio::ip::tcp::acceptor acceptor_;

        void start_accept() {
            tcp_connection::pointer new_connection = tcp_connection::create(io_context_);
            acceptor_.async_accept(new_connection->socket(), std::bind(&tcp_server::handle_accept, this, new_connection, boost::asio::placeholders::error));
        }

        void handle_accept(tcp_connection::pointer& new_connection, const boost::system::error_code& error) {
            if (!error)
            {
                new_connection->handle();
            }
            start_accept();
        }
};

int main(int argc, char *argv[]) {
    std::cout << "Starting server!" << std::endl;
    try
    {
        boost::asio::io_context io_context;
        tcp_server server(io_context, 9090);
        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}