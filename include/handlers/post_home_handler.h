#ifndef POST_HOME_HANDLER_H
#define POST_HOME_HANDLER_H

#include <iostream>            // For std::cout, std::cerr (for logging)
#include <fstream>             // For file I/O operations
#include <vector>              // For std::vector (to store data)
#include <string>              // For std::string (to handle strings)
#include <sstream>             // For string stream operations
#include <iomanip>             // For I/O manipulation
#include <filesystem>          // For filesystem operations
#include <openssl/rand.h>      // For OpenSSL random number generation
#include <fcntl.h>             // For file control options
#include <unistd.h>            // For POSIX API functions

#include "http_request.h"      // For HttpRequest class
#include "http_response.h"     // For HttpResponse class
#include "database.h"          // For Database class
#include "router.h"            // For routing requests

class PostHomeHandler {
public:
    static void post_home_handler(HttpRequest& req, HttpResponse& res, Database& db);  // Handle POST requests for home

private:
    static void handle_redirect(HttpResponse& res, const std::string& location, const std::string& link);  // Handle redirects
    static std::string get_user_id(const std::string& token, Database& db);  // Get user ID from token
    static void put_in_db(const std::string& userId, HttpRequest::UploadedFile file, Database& db);  // Store file info in the database
};

#endif // POST_HOME_HANDLER_H