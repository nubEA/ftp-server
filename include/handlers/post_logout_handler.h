#ifndef POST_LOGOUT_HANDLER_H
#define POST_LOGOUT_HANDLER_H

#include "http_request.h"  // For HttpRequest class
#include "http_response.h" // For HttpResponse class
#include "database.h"      // For Database class
#include "util.h"          // For utility functions
#include <cctype>          // For character classification functions
#include <iostream>        // For input/output streams

class PostLogoutHandler
{
public:
    static void post_logout_handler(HttpRequest &req, HttpResponse &res, Database &db); // Handles the POST logout request
private:
    static std::string get_user_id(const std::string &token, Database &db);      // Retrieves the user ID associated with the token from the database
    static void remove_cookie_db_entry(const std::string &userId, Database &db); // Removes the session entry from the database
};

#endif