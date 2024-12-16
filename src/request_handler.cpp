#include "../includes/request_handler.hpp"
#include <boost/beast/http/field.hpp>
#include <boost/beast/http/file_body.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/status.hpp>
#include <boost/beast/http/string_body.hpp>

http::response<http::string_body> RequestHandler::handle(const http::request<http::string_body>& request) {
    string_view target = request.target();

    if (target == "/") {
        //case 1: "/" -> serve angular frontend or static frontend what ever
        http::response<http::string_body> response;
        response.result(http::status::ok); 
        response.version(request.version());
        response.set(http::field::server, "Beast");
        response.set(http::field::content_type, "text/html");
        response.keep_alive();

        //todo: load angular application / plain html & js
        response.body() = "<html><h1>TEST</h1></html>";

        response.prepare_payload();
        return response;
    }

    //case 2: "/url" -> redirect to expanded url
    //case 3: neither -> redirect to 404

    return http::response<http::string_body>{http::status::bad_request, request.version()};
}