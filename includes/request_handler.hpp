#ifndef REQUEST_HANDLER_HPP
#define REQUEST_HANDLER_HPP

#include <boost/beast/http.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>
#include <variant>

using namespace boost::beast;

class RequestHandler {
public:
std::variant<
        http::response<http::string_body>,
        http::response<http::file_body>
    > handle(const http::request<http::string_body>& request);

private:
    http::response<http::string_body> BadRequest(const std::string& why);
    http::response<http::file_body> handle_file_request(const std::string& filename, boost::system::error_code& ec);
};

#endif