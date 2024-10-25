#ifndef HTTP_CONNECTION_HPP
#define HTTP_CONNECTION_HPP

#include "request_handler.hpp"
#include <boost/beast/core/flat_buffer.hpp>
#include <memory>
#include <boost/asio.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp>

using namespace std;
using namespace boost::asio;
using namespace boost::beast;

class HttpConnection : public enable_shared_from_this<HttpConnection> {
public:
    typedef shared_ptr<HttpConnection> pointer;

    static pointer create(io_context& io_context);
    ip::tcp::socket& socket();
    void process_connection();

private:
    ip::tcp::socket socket_;
    boost::beast::flat_buffer buffer_;
    http::request<http::string_body> request_;
    http::response<http::string_body> response_;
    RequestHandler request_handler_;

    void read();
    void write();

    explicit HttpConnection(io_context& io_context);
};

#endif