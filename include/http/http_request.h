#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <iostream>      // For std::cout, std::cerr
#include <string>        // For std::string
#include <cstdint>       // For std::uint8_t
#include <unordered_map> // For std::unordered_map
#include <vector>        // For std::vector

class HttpRequest
{
public:
    enum class FileType
    { // Enum to define the file type
        TEXT,
        BINARY
    };

    struct UploadedFile
    {                                        // Struct to hold the uploaded file information
        std::string filename{};              // Filename of the uploaded file
        std::vector<std::uint8_t> content{}; // File content stored as a vector of bytes
        std::string perms{"private"};        // File permissions (default: "private")
        std::string downloadLink{};          // Download link for the file
        double size{};                       // File size
        FileType type{FileType::BINARY};     // File type (default: BINARY)
    };
    UploadedFile file; // Public file object for single file uploads

    HttpRequest() = default; // Default constructor

    // Getters
    // 'const' after the function name means the function can't modify the class members
    std::string get_body() const;                                                    // Returns the body of the HTTP request
    std::unordered_map<std::string, std::string> get_headers() const;                // Returns all headers as a key-value map
    std::string get_specific_header(const std::string &key) const;                   // Returns the value of a specific header by key
    std::string get_http_version() const;                                            // Returns the HTTP version (e.g., "HTTP/1.1")
    std::string get_method() const;                                                  // Returns the HTTP method (e.g., "GET", "POST")
    std::string get_path() const;                                                    // Returns the request path (e.g., "/login")
    std::string get_unparsed_query_params() const;                                   // Returns the unparsed query parameter string (e.g., "/register?error=...")
    std::string get_query_param(const std::string &key) const;                       // Returns the value of a specific query parameter by key
    std::unordered_map<std::string, std::string> get_query_map() const;              // Returns a map of query parameters
    std::string get_token_cookie() const;                                            // Returns the token from the cookie header
    std::string get_boundary_string() const;                                         // Returns the boundary string from Content-Type header for multipart requests
    std::string get_specific_form_field(const std::string &key) const;               // Returns the value of a specific form field by key
    std::string get_file_name(const std::string &field_name) const;                  // Returns the filename of the uploaded file from the specified form field
    std::string get_file_perms(const std::string &field_name) const;                 // Returns the permissions of the uploaded file (private/public)
    std::vector<std::uint8_t> get_file_content(const std::string &field_name) const; // Returns the file content as a vector of bytes
    std::string as_text(const std::vector<std::uint8_t> &content) const;             // Converts vector of unsigned char to a string
    FileType get_file_type(const std::string &field_name) const;                     // Returns the type of the file (TEXT or BINARY)
    size_t get_file_size(const std::string &field_name) const;                       // Returns the size of the file
    std::unordered_map<std::string, UploadedFile> get_all_files() const;             // Returns a map of all uploaded files, keyed by field name
    std::unordered_map<std::string, UploadedFile> &get_all_files_ref();              // Returns a reference to the map of all uploaded files

    // Setters
    void set_method(const std::string &meth);                                        // Sets the HTTP method
    void set_http_version(const std::string &ver);                                   // Sets the HTTP version
    void set_path(const std::string &urlpath);                                       // Sets the request path
    void set_body(const std::string &reqBody);                                       // Sets the request body
    void set_header(const std::unordered_map<std::string, std::string> &head);       // Sets all headers at once
    void set_specific_header(const std::string &key, const std::string &value);      // Sets a single header key-value pair
    void set_unparsed_query_string(const std::string &qString);                      // Sets the unparsed query string
    void set_specific_query_param(const std::string &key, const std::string &value); // Sets a single query parameter key-value pair
    void set_token_cookie(const std::string &token);                                 // Sets the token
    void set_boundary_string(const std::string &boundary);                           // Sets the boundary string for multipart requests
    void set_specific_form_field(const std::string &key, const std::string &value);  // Sets a specific form field key-value pair
    void add_file(const std::string &fieldName, const std::string &filename,
                  const std::vector<std::uint8_t> &content, const HttpRequest::FileType type, const std::string &perm = "private"); // Adds an uploaded file to the request
    void update_body(const std::string &body);                                                                                      // Updates the request body

    // Utilities
    void clear();                                                // Clears all fields of the HTTP request
    bool has_body() const;                                       // Checks if the request has a non-empty body
    bool has_header(const std::string &key) const;               // Checks if a specific header exists
    bool has_query_params() const;                               // Checks if the request has query parameters
    bool has_specific_query_param(const std::string &key) const; // Checks if a specific query parameter exists
    bool has_boundary_string() const;                            // Checks if the request has a boundary string (for multipart requests)
    bool has_form_field(const std::string &key) const;           // Checks if a form field exists
    bool has_file(const std::string &field_name) const;          // Checks if a file exists for the specified field name
    bool full_body_received() const;                             // Checks if the HTTP Request contains the entire body or not

private:
    std::unordered_map<std::string, UploadedFile> uploaded_files; // Stores uploaded files keyed by field name
    std::unordered_map<std::string, std::string> form_fields;     // Stores normal form fields (e.g., name=harshit)

    std::string method{};                                       // HTTP method (e.g., "GET", "POST")
    std::string path{};                                         // Request path (e.g., "/login")
    std::string http_version{};                                 // HTTP version (e.g., "HTTP/1.1")
    std::string body{};                                         // Request body
    std::string token{};                                        // Token from the cookie header
    std::string boundaryString{};                               // Boundary string for multipart requests
    std::unordered_map<std::string, std::string> header{};      // Headers as key-value pairs
    std::string unparsedQuery{};                                // Unparsed query string
    std::unordered_map<std::string, std::string> queryParams{}; // Query parameters as key-value pairs
};

#endif // HTTP_REQUEST_H