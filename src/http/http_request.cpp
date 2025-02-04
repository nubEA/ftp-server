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

//Gets the value corresponding to a key
std::string HttpRequest::get_query_param(const std::string& key) const
{
    return queryParams.at(key);
}

std::unordered_map<std::string,std::string> HttpRequest::get_query_map() const{
    return queryParams;
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