#ifndef POST_HOME_HANDLER_H
#define POST_HOME_HANDLER_H

#include "http_request.h"
#include "http_response.h"
#include "database.h"
#include "bcrypt.h"
#include "util.h"
#include <openssl/rand.h>
#include <cctype>
#include <sstream>
#include <iostream>
#include <chrono>

class PostHomeHandler{
    public:
        static void post_home_handler(HttpRequest& req, HttpResponse& res, Database& db);
    private:
        static std::string generate_random_link();
        static void put_in_db(const std::string& userId, const std::string& filename, const size_t filesize, const std::string& link, Database& db);
        static std::string get_user_id(const std::string& token, Database& db);


};

#endif