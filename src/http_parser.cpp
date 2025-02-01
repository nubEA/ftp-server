#include "http_parser.h"

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

    parse_request_line(headerLines[0],req);                     //The first line contains method, path, http version
    parse_headers(headerLines,req);                             //Store the rest of the header part as key value pairs
    req.set_body(bodyPart);                                     //Store the body part as it is
    
    return req;
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
            line = trim(line);

            if(!line.empty()) headerLines.push_back(line);
            break;
        }

        //extract the substr before "\r\n"
        line = raw.substr(startPos,endPos-startPos);
        line = trim(line);

        if(!line.empty()) headerLines.push_back(line);

        //+2 to move past the \r\n
        startPos = endPos+2;
    }

    std::cout << "Finished splitting the header part in seperate lines\n";

    return headerLines;
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
        throw std::runtime_error("Invalid request line");
    }

    req.set_method(method);
    req.set_path(path);
    req.set_http_version(version);

    std::cout << "Parsed method, path and version\n";
}

//Parse the rest of the header lines, in the form of key value pairs
void HttpParser::parse_headers(const std::vector<std::string>& headerPart, HttpRequest& req)
{
    std::unordered_map<std::string, std::string> header;

    for(std::size_t i = 1; i < headerPart.size(); ++i)
    {   
        std::size_t pos = headerPart[i].find(":");
        if(pos == std::string::npos){
            std::cout << "Missing colon(:) on line number " << i+1 << std::endl;
            continue;
        }
        std::string key = headerPart[i].substr(0,pos);          //Splitting the string just before the ":"
        std::string value = headerPart[i].substr(pos+1);        //Spliting the string just after ":", +2 may have crashed if a key had no value

        //Remove leading and trailing whitespaces
        key = trim(key);
        std::transform(key.begin(), key.end(), key.begin(), ::tolower); //since http headers are case insensitive
        value = trim(value);

        header[key] = value;
    }
    req.set_header(header);
}

//trims trailing and leading whitespace
std::string HttpParser::trim(const std::string& line)
{
    const std::string whitespace = " \t\r\n\v";  
    std::size_t begin = line.find_first_not_of(whitespace);     //Finds first occurence of character which does not match any of the ones given
    std::size_t end = line.find_last_not_of(whitespace);
    
    if(begin == end) return "";
    else return line.substr(begin,end-begin+1);
}