#ifndef REQUEST_HANDLER_HPP
#define REQUEST_HANDLER_HPP

#include <boost/beast/http.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>

using namespace boost::beast;

class RequestHandler {
public:
    http::response<http::string_body> handle(const http::request<http::string_body>& request);

private:
};

#endif