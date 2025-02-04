#ifndef POST_LOGIN_HANDLER_H
#define POST_LOGIN_HANDLER_H

#include "http_request.h"
#include "http_response.h"
#include "database.h"
#include "bcrypt.h"
#include "util.h"
#include <cctype>
#include <sstream>
#include <iostream>

class PostLoginHandler{
    public:
        static void post_login_handler(HttpRequest& req, HttpResponse& res, Database& db);
    private:
};

#endif