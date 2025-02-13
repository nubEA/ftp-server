#ifndef POST_REGISTER_HANDLER_H
#define POST_REGISTER_HANDLER_H

#include "http_request.h"  // For HttpRequest class
#include "http_response.h" // For HttpResponse class
#include "database.h"      // For Database class
#include "bcrypt.h"        // For bcrypt hashing
#include "util.h"          // For utility functions
#include <cctype>          // For character classification functions
#include <sstream>         // For string streams
#include <iostream>        // For input/output streams

class PostRegisterHandler
{
public:
    static void post_register_handler(HttpRequest &req, HttpResponse &res, Database &db); // Handles the POST register request

private:
    struct User
    {
        std::string username{}; // User's username
        std::string password{}; // User's password
    };

    static void handle_invalid_credential_error(HttpResponse &res, const std::string &message);                               // Handles invalid credential errors
    static bool is_password_valid(const std::string &password);                                                               // Checks if the password meets the complexity requirements
    static void handle_success(HttpResponse &res);                                                                            // Handles successful registration
    static struct User get_credentials(const std::string &body, HttpResponse &res);                                           // Extracts credentials from the request body
    static bool handle_validation(const std::string &username, const std::string &password, Database &db, HttpResponse &res); // Validates the user credentials
    static bool create_user(const std::string &username, const std::string &password, Database &db, HttpResponse &res);       // Creates a new user in the database
};

#endif