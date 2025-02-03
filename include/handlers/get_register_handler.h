#ifndef GET_REGISTER_HANDER_H
#define GET_REGISTER_HANDLER_H

#include "http_request.h"
#include "http_response.h"

class GetRegisterHandler{
    public:
        static void get_register_handler(HttpRequest& req, HttpResponse& res);
    private:

};

#endif