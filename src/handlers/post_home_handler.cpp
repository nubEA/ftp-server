#include "post_home_handler.h"

void PostHomeHandler::post_home_handler(HttpRequest& req, HttpResponse& res, Database& db){
    //Get the file struct from the parsed req
    HttpRequest::UploadedFile file = req.get_all_files().at("file");

    std::string downloadLink = generate_random_link();

    put_in_db(get_user_id(req.get_token_cookie(),db), file.filename, file.size, downloadLink, file.perms, db);
}

std::string PostHomeHandler::generate_random_link()
{
    unsigned char buff[4];
    //Generating random byte string of 4 bytes
    RAND_bytes(buff,4);
    std::stringstream stream;
    std::string link{};
    //Converting byte string to hex to make it a link
    for(auto& v : buff)
    {
        stream << std::hex << std::setw(2) << std::setfill('0') << (static_cast<int>(v));
    }
    link = stream.str();
    return link;
}

std::string PostHomeHandler::get_user_id(const std::string& token, Database& db)
{
    std::cout << "Fetching User Id\n";
    std::string stmtName = "get_refresh_token";
    const char* params[] = {token.c_str()};
    int paramLen[] = {static_cast<int>(token.length())};
    int paramFormat[] = {0};
    int nParams = 1;
    auto results = db.fetch_results(stmtName,params,paramLen,paramFormat,nParams);
    
    std::cout << "Returning user id\n";

    return results[0][0];
}

void PostHomeHandler::put_in_db(const std::string& userId, const std::string& filename, const size_t filesize, 
                                const std::string& link, const std::string& permission, Database& db) 
{
    std::cout << "Trying to put file details in db\n";
    std::string stmtName = "file_insertion";

    // Convert size_t to string
    std::stringstream stream;
    stream << filesize;
    std::string fileSizeString = stream.str();

    // Include permission in the query parameters
    const char* params[] = {userId.c_str(), filename.c_str(), fileSizeString.c_str(), link.c_str(), "0", permission.c_str()};
    int paramLen[] = {static_cast<int>(userId.length()), static_cast<int>(filename.length()), 
                      static_cast<int>(fileSizeString.length()), static_cast<int>(link.length()), 
                      1, static_cast<int>(permission.length())};
    int paramFormat[] = {0, 0, 0, 0, 0, 0};  // All parameters are text
    int nParams = 6;

    if (db.execute_query(stmtName, params, paramLen, paramFormat, nParams)) {
        std::cout << "File added in db successfully\n";
    } else {
        std::cout << "Some error occurred in inserting file in db\n";
    }
}
