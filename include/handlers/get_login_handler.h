#ifndef GET_LOGIN_HANDLER_H
#define GET_LOGIN_HANDLER_H

#include "http_request.h"
#include "http_response.h"
#include "database.h"
#include "bcrypt.h"
#include "util.h"
#include <cctype>
#include <sstream>
#include <iostream>

class GetLoginHandler{
    public:
        static void get_login_handler(HttpRequest& req, HttpResponse& res, Database& db);
    private:
};

#endif