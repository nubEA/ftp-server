#ifndef ROUTER_H
#define ROUTER_H

#include "http_request.h"
#include "http_parser.h"
#include "http_response.h"
#include "get_register_handler.h"
#include "post_register_handler.h"
#include "database.h"
class Router{
    public:
        HttpResponse handle_request(HttpRequest& req, Database& db);
    private:
        void handle_get_register(HttpRequest& req, HttpResponse& res, Database& db);
        void handle_post_register(HttpRequest& req, HttpResponse& res, Database& db);
        void handle_get_login(HttpRequest& req, HttpResponse& res, Database& db);
        void handle_post_login(HttpRequest& req, HttpResponse& res, Database& db);
        void handle_not_found(HttpRequest& req, HttpRequest& res, Database& db);
};

#endif