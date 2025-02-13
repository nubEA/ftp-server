#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <string>        // For std::string
#include <unordered_map> // For std::unordered_map
#include <iostream>      // For std::cout, std::cerr

class HttpResponse
{
public:
    enum class HttpStatus
    { // Enum for HTTP status codes
        OK = 200,
        Created = 201,
        BadRequest = 400,
        NotFound = 404,
        InternalError = 500
    };

    void set_status(int code = 200, std::string message = "");         // Sets the HTTP status code and message
    void set_header(const std::string &key, const std::string &value); // Sets a header key-value pair
    void set_body(const std::string &content);                         // Sets the response body
    std::string get_body();                                            // Gets the response body
    std::string get_string_response() const;                           // Returns the complete HTTP response as a string
    static const std::unordered_map<int, std::string> codeToMessage;   // Map to convert HTTP status code to message

private:
    int status_code;                                      // HTTP status code (e.g., 200, 404)
    std::string status_message;                           // HTTP status message (e.g., "OK", "Not Found")
    std::unordered_map<std::string, std::string> headers; // HTTP headers as key-value pairs
    std::string body;                                     // HTTP response body
};

#endif