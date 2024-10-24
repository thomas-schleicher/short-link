#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include <istream>
#include <map>
#include <string>

using namespace std;

struct HttpRequest {
    string method;
    string uri;
    string version;
    map<string, string> headers;
    string body;
};

class request_parser {
public:
    HttpRequest& parse(istream& data_stream);
};

#endif