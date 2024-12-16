#include "../includes/request_handler.hpp"
#include <boost/beast/http/field.hpp>
#include <boost/beast/http/file_body.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/status.hpp>
#include <boost/beast/http/string_body.hpp>
#include <regex>

http::response<http::string_body> BadRequest(const std::string& why) {
    http::response<http::string_body> response;
    response.result(http::status::bad_request);
    response.set(http::field::server, "Beast");
    response.set(http::field::content_type, "text/html");
    response.body() = why;
    response.prepare_payload();
    return response;
}

http::response<http::string_body> RequestHandler::handle(const http::request<http::string_body>& request) {
    string_view target = request.target();
    http::verb method = request.method();
    if (target == "/") {
        if(method == http::verb::get) {
            //case 1: "/" -> serve angular frontend or static frontend what ever
            http::response<http::string_body> response;
            response.result(http::status::ok); 
            response.version(request.version());
            response.set(http::field::server, "Beast");
            response.set(http::field::content_type, "text/html");

            //todo: load angular application / plain html & js
            response.body() = "<html><h1>TEST</h1></html>";

            response.prepare_payload();
            return response;
        }
        else if (method == http::verb::post) {
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

           return BadRequest("Request is actually not bad. processing " + url);
        }
    }else {
        if(method == http::verb::get){
            http::response<http::string_body> response;
            std::string short_url = target.substr(1);

            std::string expanded_url = "https://google.com"; //todo: get expanded url from database

            response.result(http::status::moved_permanently);
            response.set(http::field::location, expanded_url);
            response.version(request.version());
            response.set(http::field::server, "Beast");
            response.body() = "Redirecting to " + expanded_url;
            response.prepare_payload();
            return response;            
        } else {
            return BadRequest("Method not allowed");
        }
    }
        
    //case 2: "/url" -> redirect to expanded url
    //case 3: neither -> redirect to 404

    return BadRequest("No rule matched.");
}