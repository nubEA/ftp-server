#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <iostream>
#include <string>
#include <unordered_map>

class HttpRequest {
public:
    HttpRequest() = default;  // Default constructor

    // Getters
    //const after the function name means that the function cant modify the class members
    std::string get_body() const;                                                   // Returns the body of the HTTP request
    std::unordered_map<std::string, std::string> get_headers() const;               // Returns all headers as a key-value map
    std::string get_specific_header(const std::string& key) const;                           // Returns the value of a specific header by key
    std::string get_http_version() const;                                           // Returns the HTTP version (e.g., "HTTP/1.1")
    std::string get_method() const;                                                 // Returns the HTTP method (e.g., "GET", "POST")
    std::string get_path() const;                                                   // Returns the request path (e.g., "/login")

    // Setters
    void set_method(const std::string& meth);                                       // Sets the HTTP method
    void set_http_version(const std::string& ver);                                  // Sets the HTTP version
    void set_path(const std::string& urlpath);                                      // Sets the request path
    void set_body(const std::string& reqBody);                                      // Sets the request body
    void set_header(const std::unordered_map<std::string, std::string>& head);      // Sets all headers at once
    void set_specific_header(const std::string& key, const std::string& value);     // Adds a single header key-value pair

    // Utilities
    void clear();                                                                   // Clears all fields of the HTTP request
    bool has_body() const;                                                          // Checks if the request has a non-empty body
    bool has_header(const std::string& key) const;                                  // Checks if a specific header exists

private:
    std::string method;                                                             // Stores the HTTP method (e.g., "GET", "POST")
    std::string path;                                                               // Stores the request path (e.g., "/login")
    std::string http_version;                                                       // Stores the HTTP version (e.g., "HTTP/1.1")
    std::string body;                                                               // Stores the request body
    std::unordered_map<std::string, std::string> header;                            // Stores headers as key-value pairs
};

#endif // HTTP_REQUEST_H