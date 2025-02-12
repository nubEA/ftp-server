#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <iostream>
#include <string>
#include <cstdint>
#include <unordered_map>
#include <vector>

class HttpRequest {
public:
    enum class FileType{
        TEXT, BINARY
    };

    struct UploadedFile{
        std::string filename{};
        std::vector<std::uint8_t> content{};     //To store binary data and to indicate to the user that content isnt necessarily text content
        std::string perms{"private"};
        std::string downloadLink{};
        double size{};
        FileType type{FileType::BINARY};   // Default type binary
    };
    UploadedFile file;                          //Public file object

    HttpRequest() = default;  // Default constructor

    // Getters
    //const after the function name means that the function cant modify the class members
    std::string get_body() const;                                                   // Returns the body of the HTTP request
    std::unordered_map<std::string, std::string> get_headers() const;               // Returns all headers as a key-value map
    std::string get_specific_header(const std::string& key) const;                  // Returns the value of a specific header by key
    std::string get_http_version() const;                                           // Returns the HTTP version (e.g., "HTTP/1.1")
    std::string get_method() const;                                                 // Returns the HTTP method (e.g., "GET", "POST")
    std::string get_path() const;                                                   // Returns the request path (e.g., "/login")
    std::string get_unparsed_query_params() const;                                  // Returns the query param (eg, "/register?error=...") 
    std::string get_query_param(const std::string& key) const;                      // Returns the a specific value for a key param
    std::unordered_map<std::string,std::string> get_query_map() const;              // Returns map of query params
    std::string get_token_cookie() const;                                           // Returns token 
    std::string get_boundary_string() const;                                        // Returns boundary string
    std::string get_specific_form_field(const std::string& key) const;              // Returns value for a normal non-file form-field
    std::string get_file_name(const std::string& field_name) const;                 // Returns file name from field-name
    std::string get_file_perms(const std::string& field_name) const;                // Returns if file is meant for public or private download
    std::vector<std::uint8_t> get_file_content(const std::string& field_name) const;     // Returns file content
    std::string as_text(const std::vector<std::uint8_t>& content) const;                 // Converts vector of unsigned char to a string
    FileType get_file_type(const std::string& field_name) const;                    // Returns if the file has binary content or text content

    size_t get_file_size(const std::string& field_name) const;                      // Return file size
    std::unordered_map<std::string, UploadedFile> get_all_files() const;            // Returns map of files
    std::unordered_map<std::string, UploadedFile>& get_all_files_ref();             // Returns map reference of files


    // Setters
    void set_method(const std::string& meth);                                       // Sets the HTTP method
    void set_http_version(const std::string& ver);                                  // Sets the HTTP version
    void set_path(const std::string& urlpath);                                      // Sets the request path
    void set_body(const std::string& reqBody);                                      // Sets the request body
    void set_header(const std::unordered_map<std::string, std::string>& head);      // Sets all headers at once
    void set_specific_header(const std::string& key, const std::string& value);     // Sets the single header key-value pair
    void set_unparsed_query_string(const std::string& qString);                     // Sets the query string
    void set_specific_query_param(const std::string& key, const std::string& value);// Sets the single query param pair
    void set_token_cookie(const std::string& token);                                // Sets the token
    void set_boundary_string(const std::string& boundary);                          // Sets the boundary string
    void set_specific_form_field(const std::string& key, const std::string& value);  // Sets the form field key,value pairs
    void add_file(const std::string& fieldName, const std::string& filename, 
                  const std::vector<std::uint8_t>& content, const HttpRequest::FileType type ,const std::string& perm = "private");
    void update_body(const std::string& body);

    // Utilities
    void clear();                                                                   // Clears all fields of the HTTP request
    bool has_body() const;                                                          // Checks if the request has a non-empty body
    bool has_header(const std::string& key) const;                                  // Checks if a specific header exists
    bool has_query_params() const;                                                  // Checks if a query param exists
    bool has_specific_query_param(const std::string& key) const;                    // Checks if a query key,value pair exits
    bool has_boundary_string() const;                                               // Checks if req has boundary string (to check malformed request)
    bool has_form_field(const std::string& key) const;                              // Checks if form_field is present;
    bool has_file(const std::string& field_name) const;                             // Checks if file exists from the map that maps fieldname to files
    bool full_body_received() const;                                // Checks if the HTTP Request contains the entire body or not


private:
    std::unordered_map<std::string, UploadedFile> uploaded_files;                    // 
    std::unordered_map<std::string, std::string> form_fields;                       // Stores normal form fields like name=harshit

    std::string method{};                                                           // Stores the HTTP method (e.g., "GET", "POST")
    std::string path{};                                                             // Stores the request path (e.g., "/login")
    std::string http_version{};                                                     // Stores the HTTP version (e.g., "HTTP/1.1")
    std::string body{};                                                             // Stores the request body
    std::string token{};                                                            // Stores the token from cookie header
    std::string boundaryString{};                                                   // Boundary string in case of multipart request
    std::unordered_map<std::string, std::string> header{};                          // Stores headers as key-value pairs
    std::string unparsedQuery{};                                                    // Stores the query (everything after "?")
    std::unordered_map<std::string, std::string> queryParams{};                     // Stores query params as key-value pairs

};


#endif // HTTP_REQUEST_H