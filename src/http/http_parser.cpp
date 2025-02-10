#include "http_parser.h"

//HANDLE BINARY DATA PARSING 

HttpRequest HttpParser::parse(const std::string& raw_request){
    
    //Split the raw request into header part and body part by splitting it at first occurence of "\r\n\r\n"
    std::string headerPart{};
    std::string bodyPart{};

    //size_t position at which the header part ends
    //If pos is npos then the entire request is header only, no body
    std::size_t pos = raw_request.find("\r\n\r\n");
    
    if(pos != std::string::npos){
        headerPart = raw_request.substr(0, pos);
        bodyPart = raw_request.substr(pos+4);                               //+4 to make sure body starts after \r\n\r\n
    }
    else{
        headerPart = raw_request;
    }

    std::cout << "Header part and body part have been split\n";

    //Seperating the header lines
    //Checking for the edgecase of there being no headers
    std::vector<std::string> headerLines = split_lines(headerPart);
    if(headerLines.empty()) throw std::runtime_error("Invalid request: no headers");

    HttpRequest req;

    parse_request_line(headerLines[0],req);                                                         //The first line contains method, path, http version
    parse_headers(headerLines,req);                                                                 //Store the rest of the header part as key value pairs
    if(req.has_header("cookie")){
        std::cout << "Started parsing cookie token!!!\n";
        parse_token_from_header(req.get_specific_header("cookie"),req);    //Extract and store token from the cookie header
    }
    else req.set_token_cookie(std::string());
    //Check if http requests contains file data
    if(req.has_header("content-type") && req.get_specific_header("content-type").starts_with("multipart/form-data")){
        // std::cout << "###HALOOO###\n";
        parse_boundary_string(req.get_specific_header("content-type"), req);
        req.set_body(bodyPart);
    }   
    else{
        std::cout << "Setting body for non-multipart form data\n";        
        req.set_body(bodyPart);                                                                         //Store the body part as it is
    }
    
    return req;
}

//Content-Type: multipart/form-data; boundary=....
void HttpParser::parse_boundary_string(const std::string& header, HttpRequest& req)
{
    size_t startPos = header.find("boundary=");
    if(startPos == std::string::npos) req.set_boundary_string(std::string());
    else{
        startPos += std::string("boundary=").length();
        std::string boundary = header.substr(startPos);
        req.set_boundary_string(boundary);
    }
}

//Splits the header part into seperate lines, like the method line, other headers
std::vector<std::string> HttpParser::split_lines(const std::string& raw)
{
    std::vector<std::string> headerLines;
    std::size_t startPos = 0;
    std::string line{};

    std::cout << "Started splitting the header part\n";

    while(true)
    {   
        //looking for the first occurence of \r\n starting from the startPos
        std::size_t endPos = raw.find("\r\n",startPos);
        if(endPos == std::string::npos){
            line = raw.substr(startPos);
            line = Util::trim(line);

            if(!line.empty()) headerLines.push_back(line);
            break;
        }

        //extract the substr before "\r\n"
        line = raw.substr(startPos,endPos-startPos);
        line = Util::trim(line);

        if(!line.empty()) headerLines.push_back(line);

        //+2 to move past the \r\n
        startPos = endPos+2;
    }

    std::cout << "Finished splitting the header part in seperate lines\n";

    return headerLines;
}

void HttpParser::parse_token_from_header(const std::string& cookie_header, HttpRequest& req)
{
    std::cout << "This is the cookie header: " << cookie_header << '\n';
    std::string token_prefix = "token=";
    size_t start_pos = cookie_header.find(token_prefix);
    if (start_pos != std::string::npos) {
        start_pos += token_prefix.length();
        size_t end_pos = cookie_header.find(";", start_pos);
        if (end_pos == std::string::npos) {
            end_pos = cookie_header.length();
        }
        req.set_token_cookie(cookie_header.substr(start_pos, end_pos - start_pos));
    }
    else req.set_token_cookie(std::string());
    std::cout << "Cookie Token set!\n";
}


void HttpParser::parse_query_params(const std::string& path, HttpRequest& req){

    std::cout << "Looking for query params to parse...\n";

    size_t queryPos = path.find("?");
    if(queryPos == std::string::npos)
    {
        std::cout << "No params found...Moving on\n";
        return;
    }

    //Setting unparsed query string
    std::string unparsedQueryString = path.substr(queryPos+1);
    req.set_unparsed_query_string(unparsedQueryString);
    
    //eg query str: q=hello+world&lang=en&sort=desc, so we can use & as delimiter and then separate from =
    std::stringstream stream(unparsedQueryString);
    std::string keyValueString{};
    
    while(getline(stream,keyValueString,'&'))
    {
        size_t equalToPos = keyValueString.find("=");
        std::string key{};
        std::string value{};
        //Extract key and value (url encoded, so we will have to decode later on) 
        if(equalToPos == std::string::npos){
            key = keyValueString;
            value = "";    
        }
        else{
            key = keyValueString.substr(0,equalToPos);
            value = keyValueString.substr(equalToPos+1);
        }
        key = Util::trim(key);
        value = Util::trim(value);
        key = Util::url_decode(key);
        value = Util::url_decode(value);
        //Set the query param
        req.set_specific_query_param(key,value);
    }
}

//Parse the first line which contains method, path and HTTP version
void HttpParser::parse_request_line(const std::string& line, HttpRequest& req)
{   
    std::string method{}, path{}, version{};
    std::istringstream stream(line);
    
    //Gets us values seperated at whitespaces
    //since http requests are url encoded if path has whitespaces they will be encoded (eg /hello%20world)
    stream >> method >> path >> version;
    
    //In case of missing method, path or version
    if (method.empty() || path.empty() || version.empty()) {
        std::cout << "Invalid request\n";
        throw std::runtime_error("Invalid request line");
    }

    req.set_method(method);
    req.set_path(path);
    req.set_http_version(version);

    parse_query_params(path,req);

    std::cout << "Parsed method, path and version\n";
}

//Parse the rest of the header lines, in the form of key value pairs
void HttpParser::parse_headers(const std::vector<std::string>& headerPart, HttpRequest& req)
{
    std::unordered_map<std::string, std::string> header;

    for(std::size_t i = 1; i < headerPart.size(); ++i)
    {   
        // std::cout << headerPart[i] << "\n\n";
        std::size_t pos = headerPart[i].find(":");
        if(pos == std::string::npos){
            std::cout << "Missing colon(:) on line number " << i+1 << std::endl;
            continue;
        }
        std::string key = headerPart[i].substr(0,pos);          //Splitting the string just before the ":"
        std::string value = headerPart[i].substr(pos+1);        //Spliting the string just after ":", +2 may have crashed if a key had no value

        //Remove leading and trailing whitespaces
        key = Util::trim(key);
        std::transform(key.begin(), key.end(), key.begin(), ::tolower); //since http headers are case insensitive
        value = Util::trim(value);

        header[key] = value;
    }
    req.set_header(header);
}