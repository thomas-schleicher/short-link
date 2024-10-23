#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>
#include <iostream>

class tcp_connection : public std::enable_shared_from_this<tcp_connection> {
    public:
        typedef std::shared_ptr<tcp_connection> pointer;

        static pointer create(boost::asio::io_context& io_context) {
            return pointer(new tcp_connection(io_context));
        }
    
        boost::asio::ip::tcp::socket& socket() {
            return socket_;
        }

        void start() {
            boost::asio::async_read_until(socket_, buffer_, "\r\n\r\n",
                std::bind(&tcp_connection::handle_read, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }

    private:
        boost::asio::ip::tcp::socket socket_;
        boost::asio::streambuf buffer_;

        explicit tcp_connection(boost::asio::io_context& io_context) : socket_(io_context) {}

        void handle_read(const boost::system::error_code& error, std::size_t bytes_transferred) {
            if (!error) {
                std::istream request_stream(&buffer_);
                std::string request_line;
                std::getline(request_stream, request_line);

                std::cout << "Received HTTP request line: " << request_line << std::endl;

                std::string header;
                while (std::getline(request_stream, header) && header != "\r") {
                    std::cout << "Header: " << header << std::endl;
                }
            }
            else {
                std::cerr << "Error on receiving request: " << error.message() << std::endl;
            }
        }

};

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
                new_connection->start();
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