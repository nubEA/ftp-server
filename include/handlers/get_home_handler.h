#ifndef GET_HOME_HANDLER_H
#define GET_HOME_HANDLER_H

#include "http_request.h"
#include "http_response.h"
#include "database.h"
#include "util.h"
#include <cctype>
#include <sstream>
#include <iostream>

class GetHomeHandler{
    public:
        static void get_home_handler(HttpRequest& req, HttpResponse& res, Database& db);
    private:
};

#endif