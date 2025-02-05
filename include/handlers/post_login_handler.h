#ifndef POST_LOGIN_HANDLER_H
#define POST_LOGIN_HANDLER_H

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

class PostLoginHandler{
    public:
        static void post_login_handler(HttpRequest& req, HttpResponse& res, Database& db);
    private:
        struct User{
            std::string username{};
            std::string password{};
        };
        static struct User get_credentials(const std::string& body, HttpResponse& res);
        static void handle_invalid_credential_error(HttpResponse& res, const std::string& message);
        static bool handle_validation(const std::string& username, const std::string& password, Database& db, HttpResponse& res, std::string& id);
        static void handle_session_token(std::string& username, Database& db, HttpResponse& res, const std::string& id);
        static void set_cookie(const std::string& token, const std::string& expiresAt, HttpResponse& res);
        static void handle_success(HttpResponse& res);

};

#endif