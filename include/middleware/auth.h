#ifndef AUTH_H
#define AUTH_H

#include "http_request.h"
#include "http_response.h"
#include "database.h"
#include "util.h"
#include "bcrypt.h"
#include <chrono>
#include <cctype>
#include <sstream>
#include <iostream>

class Auth{
    public:
        //Returns false if the user not logged in or the cookie not verified
        static bool handle_authentication(HttpRequest& req, HttpResponse& res, Database& db);
    private:
        static bool isExpired(std::string& expiresAt, std::string& hashedToken, Database& db);
        static void handle_not_logged_in(HttpRequest& req, HttpResponse& res, Database& db);
        static std::string extract_token_from_cookie(const std::string& cookie_header);
};

#endif