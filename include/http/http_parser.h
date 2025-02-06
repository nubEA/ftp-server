#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include "http_request.h"
#include "util.h"
#include <string>
#include <vector>
#include <sstream>  
#include <unordered_map>
#include <algorithm>        //for std::transform

class HttpParser {
public:
    HttpParser() = delete;                                                                      // Prevent instantiation (static methods only)
    static HttpRequest parse(const std::string& raw_request);                                   // Parses raw HTTP request into HttpRequest object

private:
    // Helper to split raw request into lines
    static std::vector<std::string> split_lines(const std::string& raw);                        // Splits raw request into individual header lines

    // Helpers to parse components
    static void parse_request_line(const std::string& line, HttpRequest& req);                  // Parses method, path, and HTTP version from request line
    static void parse_headers(const std::vector<std::string>& headerPart, HttpRequest& req);    // Parses headers into key-value pairs
    static void parse_query_params(const std::string& path, HttpRequest& req);                  // Parses the query params from path string if available
    static void parse_token_from_header(const std::string& cookie_header, HttpRequest& req);    // Parses cookie heaader to get token value
    static void parse_boundary_string(const std::string& header, HttpRequest& req);             // Parses boundary string from content-type header
    static void parse_multipart_body(const std::string& bodyPart, HttpRequest& req);

};

#endif // HTTP_PARSER_H