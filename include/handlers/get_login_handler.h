#ifndef GET_LOGIN_HANDLER_H
#define GET_LOGIN_HANDLER_H

#include "http_request.h"      // For HttpRequest class
#include "http_response.h"     // For HttpResponse class
#include "database.h"          // For Database class
#include "bcrypt.h"            // For bcrypt hashing
#include "util.h"              // For utility functions
#include <cctype>              // For character handling functions
#include <sstream>             // For string stream operations
#include <iostream>            // For std::cout, std::cerr (for logging)

class GetLoginHandler {
public:
    static void get_login_handler(HttpRequest& req, HttpResponse& res, Database& db);  // Handle login requests
private:
};

#endif // GET_LOGIN_HANDLER_H