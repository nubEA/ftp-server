#include "http_response.h"

const std::unordered_map<int, std::string> HttpResponse::codeToMessage = {
    // 2xx Success
    {200, "OK"},
    {201, "Created"},
    {202, "Accepted"},
    {204, "No Content"},
    
    // 3xx Redirection
    {301, "Moved Permanently"},
    {302, "Found"},
    {303, "See Other"},
    {304, "Not Modified"},
    
    // 4xx Client Errors
    {400, "Bad Request"},
    {401, "Unauthorized"},
    {403, "Forbidden"},
    {404, "Not Found"},
    {405, "Method Not Allowed"},
    {408, "Request Timeout"},
    {409, "Conflict"},
    {410, "Gone"},

    // 5xx Server Errors
    {500, "Internal Server Error"},
    {501, "Not Implemented"},
    {502, "Bad Gateway"},
    {503, "Service Unavailable"},
    {504, "Gateway Timeout"}
};

//Setting the status code and status message
void HttpResponse::set_status(int code, std::string message){

    if(code < 100 || code >= 600){
        throw std::runtime_error("Invalid status code");
    }

    status_code = code;
    //If messaage string is empty then we use the code to message map to fill in status message
    if(codeToMessage.count(code) == 0 && message.size() == 0){
        throw std::runtime_error("Unknown status code without message");
    }
    else status_message = (!message.empty()) ? message : codeToMessage.at(code);

    std::cout << "Successfully Set Status For the HTTP Response\n";
}

//Setting the header
void HttpResponse::set_header(const std::string& key, const std::string& value)
{
    headers[key] = value;
    std::cout << "Successfully set the headers for HTTP Response\n";
}

//Setting the body
void HttpResponse::set_body(const std::string& content)
{
    body = content;
    std::cout << "Successfully set the body for the HTTP Response\n";
}

std::string HttpResponse::get_body(){
    return body;
}

std::string HttpResponse::get_string_response() const
{
    std::string response{};
    std::string http_version = "HTTP/1.1 ";

    //Add the reponse 1st line
    response += http_version + std::to_string(status_code) + " " + status_message + "\r\n";
    //Add the headers
    for(auto& [key,value] : headers)
    {
        response += key + ": " + value + "\r\n"; 
    }
    //The empty line after headers
    response += "\r\n\r\n";
    //Add the body
    response += body;

    std::cout << "Returning the string http response....\n";
    
    return response;
}
