#ifndef ROUTER_H
#define ROUTER_H

#include "http_request.h"
#include "http_parser.h"
#include "http_response.h"
#include "get_register_handler.h"
#include "post_register_handler.h"

class Router{
    public:
        HttpResponse handle_request(HttpRequest& req);
    private:
        void handle_get_register(HttpRequest& req, HttpResponse& res);
        void handle_post_register(HttpRequest& req, HttpResponse& res);
        void handle_get_login(HttpRequest& req, HttpResponse& res);
        void handle_post_login(HttpRequest& req, HttpResponse& res);
        void handle_not_found(HttpRequest& req, HttpRequest& res);
};

#endif