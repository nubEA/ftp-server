#include "router.h"

HttpResponse Router::handle_request(HttpRequest& req, Database& db){
    HttpResponse res;
    
    //Starts with so that if the path as query parameters it still goes to the correct handler
    if(req.get_method() == "GET" && req.get_path().starts_with("/register")) handle_get_register(req, res, db);
    else if(req.get_method() == "POST" && req.get_path() == "/register") handle_post_register(req, res, db);
    // else if(req.get_method() == "GET" && req.get_path() == "/login") handle_get_login(req, res);
    // else if(req.get_method() == "POST" && req.get_path() == "/login") handle_post_login(req, res);

    return res;
}

void Router::handle_get_register(HttpRequest& req, HttpResponse& res, Database& db){
    std::cout << "Delegating the request to get register handler\n";
    GetRegisterHandler::get_register_handler(req,res,db);
}

void Router::handle_post_register(HttpRequest& req, HttpResponse& res, Database& db)
{
    std::cout << "Delegating the request to post register handler\n";
    PostRegisterHandler::post_register_handler(req,res,db);
}