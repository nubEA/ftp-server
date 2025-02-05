#ifndef POST_HOME_HANDLER_H
#define POST_HOME_HANDLER_H

#include "http_request.h"
#include "http_response.h"
#include "database.h"
#include "bcrypt.h"
#include "util.h"
#include <openssl/rand.h>
#include <cctype>
#include <sstream>
#include <iostream>
#include <chrono>

class PostHomeHandler{
    public:
        static void post_home_handler(HttpRequest& req, HttpResponse& res, Database& db);
    private:
};

#endif