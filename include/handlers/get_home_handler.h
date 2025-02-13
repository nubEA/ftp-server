#ifndef GET_HOME_HANDLER_H
#define GET_HOME_HANDLER_H

#include "http_request.h"      // For HttpRequest class
#include "http_response.h"     // For HttpResponse class
#include "database.h"          // For Database class
#include "util.h"              // For utility functions
#include <cctype>              // For character handling functions
#include <sstream>             // For string stream operations
#include <iostream>            // For std::cout, std::cerr (for logging)

class GetHomeHandler {
public:
    static void get_home_handler(HttpRequest& req, HttpResponse& res, Database& db);  // Handle home page requests
private:
};

#endif // GET_HOME_HANDLER_H