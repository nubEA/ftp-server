#include "http_request.h"

//Gets the body of the http request
std::string HttpRequest::get_body() const
{
    return body;
}

//Gets the headers in a key,value pair of the http request
std::unordered_map<std::string,std::string> HttpRequest::get_headers() const
{
    return header;
}

//get specific header for a particular key
std::string HttpRequest::get_specific_header(const std::string& key) const
{
    return header.at(key);
}

//Gets the http version of the http request
std::string HttpRequest::get_http_version() const
{
    return http_version;
}

//Gets the method of the http request
std::string HttpRequest::get_method() const
{
    return method;
}

//Gets the path of the http request
std::string HttpRequest::get_path() const
{
    return path;
}

//Gets the query param
std::string HttpRequest::get_unparsed_query_params() const{
    //Return the entire string after "?"
    return unparsedQuery;
}

//Gets the token for auth purposes
std::string HttpRequest::get_token_cookie() const{
    return token;
}

//Gets the value corresponding to a key
std::string HttpRequest::get_query_param(const std::string& key) const
{
    return queryParams.at(key);
}

std::unordered_map<std::string,std::string> HttpRequest::get_query_map() const{
    return queryParams;
}

//Gets the value of boundary string
std::string HttpRequest::get_boundary_string() const{
    return boundaryString;
}

//Return map of name to file struct
std::unordered_map<std::string, HttpRequest::UploadedFile> HttpRequest::get_all_files() const{
    return uploaded_files;
}

std::unordered_map<std::string, HttpRequest::UploadedFile>& HttpRequest::get_all_files_ref(){
    return uploaded_files;
}   

// Return specifc form field value
std::string HttpRequest::get_specific_form_field(const std::string& key) const
{
    if(form_fields.find(key) == form_fields.end()) return "";
    return form_fields.at(key);
}

std::string HttpRequest::get_file_name(const std::string& field_name) const
{
    if (has_file(field_name)) return uploaded_files.at(field_name).filename;
    return "";
}

std::string HttpRequest::get_file_perms(const std::string& field_name) const{
    if (has_file(field_name)) return uploaded_files.at(field_name).perms;
    return "";
}


std::vector<uint8_t> HttpRequest::get_file_content(const std::string& field_name) const {
    if (has_file(field_name)) return uploaded_files.at(field_name).content;
    std::vector<uint8_t> cont{};
    return cont;
}

std::string HttpRequest::as_text(const std::vector<uint8_t>& content) const{
    return std::string(content.begin(),content.end());
}

HttpRequest::FileType HttpRequest::get_file_type(const std::string& field_name) const{
    if(has_file(field_name)) return uploaded_files.at(field_name).type;
    else return HttpRequest::FileType::BINARY;
}


size_t HttpRequest::get_file_size(const std::string& field_name) const {
    if (has_file(field_name)) return uploaded_files.at(field_name).size;
    return 0;
}

//Sets the http method
void HttpRequest::set_method(const std::string& meth)
{
    method = meth;
}

//Sets the http version
void HttpRequest::set_http_version(const std::string& ver)
{
    http_version = ver;
}

//Sets the http url path
void HttpRequest::set_path(const std::string& urlpath){
    path = urlpath;
}

//Sets the body
void HttpRequest::set_body(const std::string& reqBody){
    body = reqBody;
}

//Sets the header
void HttpRequest::set_header(const std::unordered_map<std::string, std::string>& head)
{
    header = head;
}

//Set specific header value
void HttpRequest::set_specific_header(const std::string& key, const std::string& value)
{
    header[key] = value;
}

//Set the unparsed Query string
void HttpRequest::set_unparsed_query_string(const std::string& qString)
{
    unparsedQuery = qString;
}

//Set specific query param
void HttpRequest::set_specific_query_param(const std::string& key, const std::string& value)
{
    queryParams[key] = value;
}

//Set the token
void HttpRequest::set_token_cookie(const std::string& tokenFromCookie)
{
    token = tokenFromCookie;
}

//Set the boundary string which seperates the body in multipart http request
void HttpRequest::set_boundary_string(const std::string& boundary)
{
    boundaryString = boundary;
}

void HttpRequest::set_specific_form_field(const std::string& field_name, const std::string& value) {
    form_fields[field_name] = value;
}

void HttpRequest::add_file(const std::string& fieldName, const std::string& filename, const std::vector<uint8_t>& content, const HttpRequest::FileType type ,const std::string& perm){
    uploaded_files[fieldName] = {filename, content, perm, content.size(), type};
}

//Clear the http request obj
void HttpRequest::clear(){
    method.clear();
    path.clear();
    http_version.clear();
    body.clear();
    header.clear();
}

//check if body exits
bool HttpRequest::has_body() const
{
    return !body.empty();
}

//check if it has a specific header
bool HttpRequest::has_header(const std::string& key) const
{
    return header.count(key);
}

//Check if query param exist
bool HttpRequest::has_query_params() const
{
    return (body.find("?") != std::string::npos);
}

//Check if specific key,value pair exists
bool HttpRequest::has_specific_query_param(const std::string& key) const
{
    return queryParams.count(key);
}

//Check if boundary string is present or not
bool HttpRequest::has_boundary_string() const{
    return !boundaryString.empty();
}

bool HttpRequest::has_form_field(const std::string& field_name) const {
    return form_fields.find(field_name) != form_fields.end();
}

bool HttpRequest::has_file(const std::string& field_name) const {
    return uploaded_files.find(field_name) != uploaded_files.end();
}
