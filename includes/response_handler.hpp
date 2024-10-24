#ifndef RESPONSE_HANDLER_HPP
#define RESPONSE_HANDLER_HPP

#include "request_parser.hpp"
#include <string>

using namespace std;

class response_handler {
public:
    string& handle(HttpRequest request);
};

#endif