#ifndef UTIL_H
#define UTIL_H

#include <algorithm>
#include <string>
#include <vector>
#include "http_request.h"
#include "http_response.h"

namespace Util {
    std::string trim(const std::string& str);
    std::string to_lower(const std::string& str);
    std::string to_upper(const std::string& str);
    std::vector<std::string> split_string(const std::string& str, char delimiter);
    bool is_digit(const std::string& str);

    // URL Encoding and Decoding
    std::string char_to_hex(char c);
    std::string url_encode(const std::string& str);
    std::string url_decode(const std::string& str);

    // Validation Helpers
    bool has_upper_case(const std::string& password);
    bool has_lower_case(const std::string& password);
    bool has_digit(const std::string& password);
    bool is_username_valid(const std::string& username);

    // HTTP Error Helpers
    void make_error_response(int code, const std::string& message, HttpResponse& res);
}

#endif // UTIL_H
