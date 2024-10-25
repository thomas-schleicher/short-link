#include "../includes/request_handler.hpp"
#include <boost/beast/http/field.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/status.hpp>
#include <boost/beast/http/string_body.hpp>

http::response<http::string_body> RequestHandler::handle(const http::request<http::string_body>& request) {
    http::response<http::string_body> response{http::status::ok, request.version()};
    response.set(http::field::server, "Beast");
    response.set(http::field::content_type, "text/plain");
    response.body() = "Hello, World!";
    response.prepare_payload();
    return response;
}