#ifndef POST_HOME_HANDLER_H
#define POST_HOME_HANDLER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <openssl/rand.h>
#include <fcntl.h>
#include <unistd.h>

#include "http_request.h"
#include "http_response.h"
#include "database.h"

class PostHomeHandler {
public:
    static void post_home_handler(HttpRequest& req, HttpResponse& res, Database& db);
    
private:
    static void store_file_locally(HttpRequest& req, HttpRequest::UploadedFile& file, std::string hashedFileName);
    static bool write_text_file(const std::string& tempPath, const std::string& finalPath, const std::string& content);
    static bool write_binary_file(const std::string& tempPath, const std::string& finalPath, const std::vector<std::uint8_t>& content);
    
    static std::string generate_random_link();
    static std::string get_user_id(const std::string& token, Database& db);
    static void put_in_db(const std::string& userId, HttpRequest::UploadedFile file, const std::string& link, Database& db);
};

#endif // POST_HOME_HANDLER_H
