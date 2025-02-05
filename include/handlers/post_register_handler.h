#ifndef POST_REGISTER_HANDLER_H
#define POST_REGISTER_HANDLER_H

#include "http_request.h"
#include "http_response.h"
#include "database.h"
#include "bcrypt.h"
#include "util.h"
#include <cctype>
#include <sstream>
#include <iostream>

class PostRegisterHandler {
public:
    static void post_register_handler(HttpRequest& req, HttpResponse& res, Database& db);

private:
    struct User {
        std::string username{};
        std::string password{};
    };

    static void handle_invalid_credential_error(HttpResponse& res, const std::string& message);
    static bool is_password_valid(const std::string& password); 
    static void handle_success(HttpResponse& res);
    static struct User get_credentials(const std::string& body, HttpResponse& res); 
    static bool handle_validation(const std::string& username, const std::string& password, Database& db, HttpResponse& res); 
    static bool create_user(const std::string& username, const std::string& password, Database& db, HttpResponse& res); 
};

#endif