#include "post_home_handler.h"
#define EIGHT_KB 8192

void PostHomeHandler::post_home_handler(HttpRequest& req, HttpResponse& res, Database& db){
    //Get the file struct from the parsed req
    HttpRequest::UploadedFile file = req.file;
    put_in_db(get_user_id(req.get_token_cookie(),db), file, db);
    std::cout << "Exiting post home handler\n";
    std::string queryParam = "/download/" + file.downloadLink;
    handle_redirect(res, "/?link=",queryParam);
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

void PostHomeHandler::put_in_db(const std::string& userId, HttpRequest::UploadedFile file, Database& db) 
{
    std::cout << "Trying to put file details in db\n";

    //Preparing all necessary params for file insertion in db
    std::string stmtName = "file_insertion";
    std::string filename = file.filename;
    size_t filesize = file.size;
    std::string permission = file.perms;
    std::string isCompressed = "0";
    std::string filetype = (file.type == HttpRequest::FileType::TEXT) ? "text" : "binary";
    std::string fileSizeString = std::to_string(filesize);
    std::string link = file.downloadLink;
    
    // Include permission in the query parameters
    const char* params[] = {userId.c_str(), filename.c_str(), fileSizeString.c_str(), link.c_str(), isCompressed.c_str(), permission.c_str(), 
                            filetype.c_str()};
    int paramLen[] = {static_cast<int>(userId.length()), static_cast<int>(filename.length()), 
                      static_cast<int>(fileSizeString.length()), static_cast<int>(link.length()), 
                      static_cast<int>(isCompressed.length()), static_cast<int>(permission.length()), static_cast<int>(filetype.length())};
    int paramFormat[] = {0, 0, 0, 0, 0, 0, 0};  // All parameters are text
    int nParams = 7;

    if (db.execute_query(stmtName, params, paramLen, paramFormat, nParams)) {
        std::cout << "File added in db successfully\n";
    } else {
        std::cout << "Some error occurred in inserting file in db\n";
    }
}

void PostHomeHandler::handle_redirect(HttpResponse& res, const std::string& location, const std::string& link)
{
    res.set_status(303, "See Other");
    std::string queryParam = Util::url_encode(link);
    res.set_header("Location",location + queryParam);
}
