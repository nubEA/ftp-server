#ifndef POST_REGISTER_HANDLER_H
#define POST_REGISTER_HANDLER_H

#include "http_request.h"
#include "http_response.h"
#include "database.h"
#include "util.h"
#include <cctype>
#include <sstream>
#include <iostream>

class PostRegisterHandler{
    public:
        static void post_register_handler(HttpRequest& req, HttpResponse& res);
    private:
        static void handle_invalid_credential_error(HttpResponse& res);
        static bool is_password_valid(std::string& password);

};

#endif