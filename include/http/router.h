#ifndef ROUTER_H
#define ROUTER_H

#include "http_request.h"          // For HttpRequest class
#include "http_parser.h"           // For HttpParser class
#include "http_response.h"         // For HttpResponse class
#include "get_register_handler.h"  // For GetRegisterHandler
#include "post_register_handler.h" // For PostRegisterHandler
#include "get_login_handler.h"     // For GetLoginHandler
#include "post_login_handler.h"    // For PostLoginHandler
#include "get_home_handler.h"      // For GetHomeHandler
#include "post_home_handler.h"     // For PostHomeHandler
#include "post_logout_handler.h"   // For PostLogoutHandler
#include "get_history_handler.h"   // For GetHistoryHandler
#include "get_download_handler.h"  // For GetDownloadHandler
#include "auth.h"                  // For authentication related functions
#include "database.h"              // For Database class

class Router
{
public:
    HttpResponse handle_request(HttpRequest &req, Database &db, int clientSocketFd); // Handles the incoming HTTP request and returns a response
private:
    void handle_get_register(HttpRequest &req, HttpResponse &res, Database &db);         // Handles GET request to the registration page
    void handle_post_register(HttpRequest &req, HttpResponse &res, Database &db);        // Handles POST request for user registration
    void handle_get_login(HttpRequest &req, HttpResponse &res, Database &db);            // Handles GET request to the login page
    void handle_post_login(HttpRequest &req, HttpResponse &res, Database &db);           // Handles POST request for user login
    void handle_get_history(HttpRequest &req, HttpResponse &res, Database &db);          // Handles GET request to the history page
    void handle_get_home(HttpRequest &req, HttpResponse &res, Database &db);             // Handles GET request to the home page
    void handle_post_home(HttpRequest &req, HttpResponse &res, Database &db);            // Handles POST request to the home page
    void handle_not_found(HttpRequest &req, HttpRequest &res, Database &db);             // Handles 404 Not Found errors
    void handle_post_logout(HttpRequest &req, HttpResponse &res, Database &db);          // Handles POST request for user logout
    void handle_redirect(HttpResponse &res, const std::string &location);                // Handles redirection to a specified location
    void handle_get_download(HttpRequest &req, HttpResponse &res, Database &db, int fd); // Handles GET request to download a file
};

#endif