#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <string>
#include <unordered_map>
#include <iostream>

class HttpResponse{
    public:
        enum class HttpStatus {
            OK = 200,
            Created = 201,
            BadRequest = 400,
            NotFound = 404,
            InternalError = 500
        };

        void set_status(int code = 200, std::string message = "");
        void set_header(const std::string& key, const std::string& value);
        void set_body(const std::string& content);
        std::string get_string_response() const;
        static const std::unordered_map<int, std::string> codeToMessage;
    private:
        int status_code;
        std::string status_message;
        std::unordered_map<std::string, std::string> headers;
        std::string body;
};

#endif