#ifndef AUTH_H
#define AUTH_H

#include "http_request.h"  // For HttpRequest class
#include "http_response.h" // For HttpResponse class
#include "database.h"      // For Database class
#include "util.h"          // For utility functions
#include "bcrypt.h"        // For bcrypt hashing
#include <chrono>          // For time-related functionalities
#include <cctype>          // For character classification functions
#include <sstream>         // For string streams
#include <iostream>        // For input/output streams

class Auth
{
public:
    // Returns false if the user is not logged in or the cookie cannot be verified
    static bool handle_authentication(HttpRequest &req, HttpResponse &res, Database &db);
    static std::string get_user_id(const std::string &token, Database &db); // Gets the user ID associated with the token from the database

private:
    static bool isExpired(std::string &expiresAt, std::string &hashedToken, Database &db); // Checks if the session token has expired
    static void handle_not_logged_in(HttpRequest &req, HttpResponse &res, Database &db);   // Handles the case when the user is not logged in
    static void remove_cookie_db_entry(const std::string &userId, Database &db);           // Removes the session cookie entry from the database
};

#endif