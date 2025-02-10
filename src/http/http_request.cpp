#include "http_request.h"

// Gets the body of the HTTP request
std::string HttpRequest::get_body() const {
    return body;
}

// Gets all headers in a key-value pair of the HTTP request
std::unordered_map<std::string, std::string> HttpRequest::get_headers() const {
    return header;
}

// Gets a specific header value for a particular key
std::string HttpRequest::get_specific_header(const std::string& key) const {
    return header.contains(key) ? header.at(key) : "";
}

// Gets the HTTP version of the HTTP request
std::string HttpRequest::get_http_version() const {
    return http_version;
}

// Gets the HTTP method of the request
std::string HttpRequest::get_method() const {
    return method;
}

// Gets the path of the HTTP request
std::string HttpRequest::get_path() const {
    return path;
}

// Gets the full unparsed query parameters (everything after "?")
std::string HttpRequest::get_unparsed_query_params() const {
    return unparsedQuery;
}

// Gets the authentication token from cookies
std::string HttpRequest::get_token_cookie() const {
    return token;
}

// Gets the value corresponding to a key from the query parameters
std::string HttpRequest::get_query_param(const std::string& key) const {
    return queryParams.contains(key) ? queryParams.at(key) : "";
}

// Gets all query parameters as a key-value map
std::unordered_map<std::string, std::string> HttpRequest::get_query_map() const {
    return queryParams;
}

// Gets the boundary string for multipart form-data requests
std::string HttpRequest::get_boundary_string() const {
    return boundaryString;
}

// Gets all uploaded files mapped by field names
std::unordered_map<std::string, HttpRequest::UploadedFile> HttpRequest::get_all_files() const {
    return uploaded_files;
}

// Gets a reference to the map of uploaded files
std::unordered_map<std::string, HttpRequest::UploadedFile>& HttpRequest::get_all_files_ref() {
    return uploaded_files;
}

// Gets a specific form field value
std::string HttpRequest::get_specific_form_field(const std::string& key) const {
    return form_fields.contains(key) ? form_fields.at(key) : "";
}

// Gets the filename for a given field name in a multipart request
std::string HttpRequest::get_file_name(const std::string& field_name) const {
    return has_file(field_name) ? uploaded_files.at(field_name).filename : "";
}

// Gets file permissions (public/private) for a given field name
std::string HttpRequest::get_file_perms(const std::string& field_name) const {
    return has_file(field_name) ? uploaded_files.at(field_name).perms : "private";
}

// Gets the binary content of the uploaded file
std::vector<uint8_t> HttpRequest::get_file_content(const std::string& field_name) const {
    return has_file(field_name) ? uploaded_files.at(field_name).content : std::vector<uint8_t>{};
}

// Converts binary file content into a string representation
std::string HttpRequest::as_text(const std::vector<uint8_t>& content) const {
    return std::string(content.begin(), content.end());
}

// Gets the file type (TEXT/BINARY) for a given field name
HttpRequest::FileType HttpRequest::get_file_type(const std::string& field_name) const {
    return has_file(field_name) ? uploaded_files.at(field_name).type : HttpRequest::FileType::BINARY;
}

// Gets the file size for a given field name
size_t HttpRequest::get_file_size(const std::string& field_name) const {
    return has_file(field_name) ? uploaded_files.at(field_name).size : 0;
}

// Sets the HTTP method
void HttpRequest::set_method(const std::string& meth) {
    method = meth;
}

// Sets the HTTP version
void HttpRequest::set_http_version(const std::string& ver) {
    http_version = ver;
}

// Sets the request path (URL)
void HttpRequest::set_path(const std::string& urlpath) {
    path = urlpath;
}

// Sets the body of the HTTP request
void HttpRequest::set_body(const std::string& reqBody) {
    body = reqBody;
}

// Sets all headers at once
void HttpRequest::set_header(const std::unordered_map<std::string, std::string>& head) {
    header = head;
}

// Sets a specific header key-value pair
void HttpRequest::set_specific_header(const std::string& key, const std::string& value) {
    header[key] = value;
}

// Sets the unparsed query string
void HttpRequest::set_unparsed_query_string(const std::string& qString) {
    unparsedQuery = qString;
}

// Sets a specific query parameter
void HttpRequest::set_specific_query_param(const std::string& key, const std::string& value) {
    queryParams[key] = value;
}

// Sets the authentication token from the cookie
void HttpRequest::set_token_cookie(const std::string& tokenFromCookie) {
    token = tokenFromCookie;
}

// Sets the boundary string for multipart form-data requests
void HttpRequest::set_boundary_string(const std::string& boundary) {
    boundaryString = boundary;
}

// Sets a specific form field key-value pair
void HttpRequest::set_specific_form_field(const std::string& field_name, const std::string& value) {
    form_fields[field_name] = value;
}

// Updates the request body
void HttpRequest::update_body(const std::string& newBody) {
    body = newBody;
}

// Clears the entire HTTP request object
void HttpRequest::clear() {
    method.clear();
    path.clear();
    http_version.clear();
    body.clear();
    token.clear();
    boundaryString.clear();
    header.clear();
    queryParams.clear();
    form_fields.clear();
    uploaded_files.clear();
}

// Checks if the request contains a non-empty body
bool HttpRequest::has_body() const {
    return !body.empty();
}

// Checks if a specific header exists
bool HttpRequest::has_header(const std::string& key) const {
    return header.contains(key);
}

// Checks if the request contains any query parameters
bool HttpRequest::has_query_params() const {
    return !queryParams.empty();
}

// Checks if a specific query parameter exists
bool HttpRequest::has_specific_query_param(const std::string& key) const {
    return queryParams.contains(key);
}

// Checks if a boundary string exists (to detect malformed multipart requests)
bool HttpRequest::has_boundary_string() const {
    return !boundaryString.empty();
}

// Checks if a specific form field exists
bool HttpRequest::has_form_field(const std::string& field_name) const {
    return form_fields.contains(field_name);
}

// Checks if a file exists for a given field name
bool HttpRequest::has_file(const std::string& field_name) const {
    return uploaded_files.contains(field_name);
}

// Checks if the entire request body has been received
bool HttpRequest::full_body_received() const {
    // No body is required for GET requests
    if (method == "GET") return true;

    // Check if the Content-Length header is present
    if (!header.contains("content-length")) return false;

    try {
        size_t expectedLength = std::stoul(header.at("content-length"));
        return body.length() == expectedLength;
    } catch (const std::exception&) {
        return false; // Invalid content-length value
    }
}
