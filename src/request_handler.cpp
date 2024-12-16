#include "../includes/request_handler.hpp"
#include <boost/beast/http/field.hpp>
#include <boost/beast/http/file_body.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/status.hpp>
#include <boost/beast/http/string_body.hpp>
#include <regex>
#include "request_handler.hpp"

http::response<http::string_body> RequestHandler::BadRequest(const std::string& why) {
    http::response<http::string_body> response;
    response.result(http::status::bad_request);
    response.set(http::field::server, "Beast");
    response.set(http::field::content_type, "text/html");
    response.body() = why;
    response.keep_alive(false);
    response.prepare_payload();
    return response;
}

std::variant<
        http::response<http::string_body>,
        http::response<http::file_body>
    > 
    RequestHandler::handle(const http::request<http::string_body>& request) {
    std::string target = request.target();
    http::verb method = request.method();

    if(method == http::verb::post){
        if(target != "/") {
            return BadRequest("Cannot post to anything other than /");
        }
        if(request.find(http::field::content_type) == request.end()) {
                return BadRequest("Content-Type header is required for POST requests");
            }
            auto content_type = request[http::field::content_type];
            if(content_type != "text/plain") {
                return BadRequest("Content-Type must be text/plain");
            }
            std::string url = request.body();
            std::regex url_regex("^(https?://)?(?:www\\.)?[-a-zA-Z0-9@%._\\+~#=]{1,256}\\.[a-zA-Z0-9()]{1,6}\\b(?:[-a-zA-Z0-9()@:%_\\+.~#?&/=]*)$");
            std::smatch url_match;
            if(!std::regex_match(url, url_match, url_regex)) {
                return BadRequest("Invalid URL");
            }
            if(!url_match[1].matched){
                url = "https://" + url;
            }

            //todo: save url to database and return short url
            http::response<http::string_body> response;
            response.result(http::status::created);
            response.set(http::field::server, "Beast");
            response.set(http::field::content_type, "text/plain");
            response.body() = "127.0.0.1:8080/asdf";
            response.keep_alive(false);
            response.prepare_payload();
            return response;
    } else if (method == http::verb::get) {
        if(target == "/"){
            target = "/index.html";
        }
        if(target == "/index.html" || target == "/index.js") {
            error_code ec;
            http::response<http::file_body> response = handle_file_request("frontend" + target, ec);
            if(ec) {
                return BadRequest("Error reading file");
            }else {
                return response;
            }
        } else {
            http::response<http::string_body> response;
            std::string short_url = target.substr(1);

            std::string expanded_url = "https://google.com"; //todo: get expanded url from database

            response.result(http::status::moved_permanently);
            response.set(http::field::location, expanded_url);
            response.version(request.version());
            response.set(http::field::server, "Beast");
            response.body() = "Redirecting to " + expanded_url;
            response.keep_alive(false);
            response.prepare_payload();
            return response;
        }

    }
    
    return BadRequest("No rule matched.");
}
http::response<http::file_body> RequestHandler::handle_file_request(const std::string& path, error_code &ec) {
    http::file_body::value_type file;
    file.open(path.c_str(), file_mode::read, ec);
    http::response<http::file_body> response;
    if(!ec){
        response.result(http::status::ok);
        response.set(http::field::server, "Beast");
        response.set(http::field::content_type, "text/html");
        response.body() = std::move(file);
        response.keep_alive(false);
        response.prepare_payload();
    };
    return response;

}