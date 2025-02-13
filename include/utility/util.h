#ifndef UTIL_H
#define UTIL_H

#include <algorithm>       // For std::transform, std::remove_if
#include <string>          // For std::string
#include <vector>          // For std::vector
#include "http_request.h"  // For HttpRequest class
#include "http_response.h" // For HttpResponse class

namespace Util
{
    std::string trim(const std::string &str);                                      // Trims whitespace from the beginning and end of a string
    std::string to_lower(const std::string &str);                                  // Converts a string to lowercase
    std::string to_upper(const std::string &str);                                  // Converts a string to uppercase
    std::vector<std::string> split_string(const std::string &str, char delimiter); // Splits a string into a vector of strings using a delimiter
    bool is_digit(const std::string &str);                                         // Checks if a string contains only digits

    // URL Encoding and Decoding
    std::string char_to_hex(char c);                // Converts a character to its hexadecimal representation
    std::string url_encode(const std::string &str); // Encodes a string for use in a URL
    std::string url_decode(const std::string &str); // Decodes a URL-encoded string

    // Validation Helpers
    bool has_upper_case(const std::string &password);    // Checks if a string has at least one uppercase letter
    bool has_lower_case(const std::string &password);    // Checks if a string has at least one lowercase letter
    bool has_digit(const std::string &password);         // Checks if a string has at least one digit
    bool is_username_valid(const std::string &username); // Checks if a username is valid

    // HTTP Error Helpers
    void make_error_response(int code, const std::string &message, HttpResponse &res); // Creates a standard HTTP error response
}

#endif // UTIL_H