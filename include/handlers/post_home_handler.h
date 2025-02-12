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
#include "router.h"

class PostHomeHandler {
public:
    static void post_home_handler(HttpRequest& req, HttpResponse& res, Database& db);
    
private:
    static void handle_redirect(HttpResponse& res, const std::string& location, const std::string& link);
    static std::string get_user_id(const std::string& token, Database& db);
    static void put_in_db(const std::string& userId, HttpRequest::UploadedFile file, Database& db);
};

#endif // POST_HOME_HANDLER_H
