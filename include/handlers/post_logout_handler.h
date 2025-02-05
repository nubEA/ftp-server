#ifndef POST_LOGOUT_HANDLER_H
#define POST_LOGOUT_HANDLER_H

#include "http_request.h"
#include "http_response.h"
#include "database.h"
#include "util.h"
#include <cctype>
#include <iostream>

class PostLogoutHandler{
    public:
        static void post_logout_handler(HttpRequest& req, HttpResponse& res, Database& db);
    private:
        static std::string get_user_id(const std::string& token, Database& db);
        static void remove_cookie_db_entry(const std::string& userId, Database& db);
};

#endif 