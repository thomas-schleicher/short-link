#include "../includes/request_handler.hpp"
#include "../includes/database_service.hpp"
#include "../includes/log_utils.hpp"
#include <boost/beast/http/field.hpp>
#include <boost/beast/http/file_body.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/status.hpp>
#include <boost/beast/http/string_body.hpp>
#include <regex>
#include <future>
#include <filesystem>

http::response<http::string_body> RequestHandler::BadRequest(const std::string& why) {
    LOG(LogLevel::BADREQUEST, "BadRequest: " + why);
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
> RequestHandler::handle(const http::request<http::string_body>& request) {
    std::string target = request.target();
    http::verb method = request.method();

    auto& dbService = DatabaseService::getInstance("urls.db", 4);

    LOG(LogLevel::INFO, "Received request:");
    LOG(LogLevel::INFO, "\tMethod: " + std::string(request.method_string()));
    LOG(LogLevel::INFO, "\tTarget: " + target);
    LOG(LogLevel::INFO, "\tBody: " + request.body());

    if (method == http::verb::post) {
        if (target != "/") {
            return BadRequest("Cannot post to anything other than /");
        }
        if (request.find(http::field::content_type) == request.end()) {
            return BadRequest("Content-Type header is required for POST requests");
        }
        auto content_type = request[http::field::content_type];
        if (content_type != "text/plain") {
            return BadRequest("Content-Type must be text/plain");
        }

        std::string url = request.body();
        std::regex url_regex("^(https?://)?(?:www\\.)?[-a-zA-Z0-9@%._\\+~#=]{1,256}\\.[a-zA-Z0-9()]{1,6}\\b(?:[-a-zA-Z0-9()@:%_\\+.~#?&/=]*)$");
        std::smatch url_match;
        if (!std::regex_match(url, url_match, url_regex)) {
            return BadRequest("Invalid URL");
        }
        if (!url_match[1].matched) {
            url = "https://" + url;
        }

        LOG(LogLevel::INFO, "Valid URL: " + url);

        std::promise<std::string> promise;
        std::future<std::string> future = promise.get_future();
        dbService.shortenURL(url, [&promise](std::error_code ec, const std::string& shortURL) {
            if (!ec) {
                promise.set_value(shortURL);
            } else {
                promise.set_exception(std::make_exception_ptr(
                    std::runtime_error(ec.message())
                ));
            }
        });

        try {
            std::string shortURL = future.get();
            LOG(LogLevel::INFO, "Shortened URL generated: " + shortURL);
            http::response<http::string_body> response;
            response.result(http::status::created);
            response.set(http::field::server, "Beast");
            response.set(http::field::content_type, "text/plain");
            response.body() = "127.0.0.1:8080/" + shortURL;
            response.keep_alive(false);
            response.prepare_payload();
            return response;
        } catch (const std::exception& e) {
            return BadRequest("Error generating short URL: " + std::string(e.what()));
        }

    }
    if (method == http::verb::get) {
        if (target == "/") {
            LOG(LogLevel::INFO, "Serving the index.html file for target /");
            target = "/index.html";
        }
        if (target == "/index.html" || target == "/index.js") {
            error_code ec;
            http::response<http::file_body> response = handle_file_request("frontend" + target, ec);
            if (ec) {
                LOG(LogLevel::ERROR, "Failed to read file: frontend" + target);
                return BadRequest("Error reading file");
            } else {
                LOG(LogLevel::INFO, "Served file: frontend" + target);
                return response;
            }
        }

        std::regex regex_pattern(".*/([^/]+)$");
        std::smatch match;
        std::string shortCode;

        if (std::regex_match(target, match, regex_pattern)) {
            shortCode = match[1];
            LOG(LogLevel::INFO, "Extracted short code using regex: " + shortCode);
        } else {
            LOG(LogLevel::ERROR, "Failed to extract short code from target: " + target);
            return BadRequest("Invalid short URL format");
        }

        std::promise<std::string> promise;
        std::future<std::string> future = promise.get_future();

        dbService.getLongURL(shortCode, [&promise](std::error_code ec, const std::string& longURL) {
            if (!ec) {
                promise.set_value(longURL);
            } else {
                promise.set_exception(std::make_exception_ptr(
                    std::runtime_error(ec.message())
                ));
            }
        });

        try {
            std::string expandedURL = future.get();
            LOG(LogLevel::INFO, "Expanded URL: " + expandedURL);
            http::response<http::string_body> response;
            response.result(http::status::moved_permanently);
            response.set(http::field::location, expandedURL);
            response.version(request.version());
            response.set(http::field::server, "Beast");
            response.body() = "Redirecting to " + expandedURL;
            response.keep_alive(false);
            response.prepare_payload();
            return response;
        } catch (const std::exception& e) {
            LOG(LogLevel::ERROR, "Failed to expand short URL: " + shortCode + ". Error: " + std::string(e.what()));
            return BadRequest("Short URL not found or error: " + std::string(e.what()));
        }
    }

    return BadRequest("No rule matched.");
}

http::response<http::file_body> RequestHandler::handle_file_request(const std::string& path, error_code& ec) {
    http::file_body::value_type file;
    file.open(path.c_str(), file_mode::read, ec);
    if (!ec) {
        LOG(LogLevel::INFO, "Successfully opened file: " + path);
    } else {
        LOG(LogLevel::ERROR, "Failed to open file: " + path + ". Error: " + ec.message());
    }

    http::response<http::file_body> response;
    if (!ec) {
        response.result(http::status::ok);
        response.set(http::field::server, "Beast");
        response.set(http::field::content_type, "text/html");
        response.body() = std::move(file);
        response.keep_alive(false);
        response.prepare_payload();
    }
    return response;
}