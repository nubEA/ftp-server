#ifndef GET_HISTORY_HANDLER_H
#define GET_HISTORY_HANDLER_H

#include "http_request.h"      // For HttpRequest class
#include "http_response.h"     // For HttpResponse class
#include "database.h"          // For Database class
#include "util.h"              // For utility functions
#include <cctype>              // For character handling functions
#include <sstream>             // For string stream operations
#include <iostream>            // For std::cout, std::cerr (for logging)
#include <algorithm>           // For algorithm functions

class GetHistoryHandler {
public:
    static void get_history_handler(HttpRequest& req, HttpResponse& res, Database& db);  // Handle history requests

private:
    static std::vector<std::vector<std::string>> get_table_content(const std::string& userId, Database& db);  // Get table content for a user
    static std::string get_user_id(const std::string& token, Database& db);  // Get user ID from token
};

#endif // GET_HISTORY_HANDLER_H