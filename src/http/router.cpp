#include "router.h"

HttpResponse Router::handle_request(HttpRequest& req, Database& db){
    HttpResponse res;
    
    //Starts with so that if the path as query parameters it still goes to the correct handler
    if(req.get_method() == "GET"        && req.get_path().starts_with("/register")) handle_get_register(req, res, db);
    else if(req.get_method() == "POST"  && req.get_path().starts_with("/register")) handle_post_register(req, res, db);
    else if(req.get_method() == "GET"   && req.get_path().starts_with("/login"))    handle_get_login(req, res, db);
    else if(req.get_method() == "POST"  && req.get_path().starts_with("/login"))    handle_post_login(req, res,db);
    else if(req.get_method() == "POST"  && req.get_path().starts_with("/logout"))   handle_post_logout(req,res,db);
    else if(req.get_method() == "GET"   && req.get_path().starts_with("/"))         handle_get_home(req,res,db);
    else if(req.get_method() == "POST"  && req.get_path().starts_with("/"))         handle_post_home(req,res,db);
    else handle_get_home(req,res,db);

    return res;
}

void Router::handle_get_register(HttpRequest& req, HttpResponse& res, Database& db){
    std::cout << "Checking authentication\n";
    if(Auth::handle_authentication(req,res,db)){
        handle_redirect(res,"/");
        return;
    }
    std::cout << "Delegating the request to GET register handler\n";
    GetRegisterHandler::get_register_handler(req,res,db);
}

void Router::handle_post_register(HttpRequest& req, HttpResponse& res, Database& db)
{
    std::cout << "Delegating the request to POST register handler\n";
    PostRegisterHandler::post_register_handler(req,res,db);
}

void Router::handle_get_login(HttpRequest& req, HttpResponse& res, Database& db)
{
    std::cout << "Checking authentication\n";
    if(Auth::handle_authentication(req,res,db)){
        handle_redirect(res,"/");
        return;
    }
    std::cout << "Delegating the request to GET login handler\n";
    GetLoginHandler::get_login_handler(req,res,db);
}

void Router::handle_post_login(HttpRequest& req, HttpResponse& res, Database& db){
    std::cout << "Delegating the request to POST login handler\n";
    PostLoginHandler::post_login_handler(req,res,db);
}

void Router::handle_get_home(HttpRequest& req, HttpResponse& res, Database& db)
{
    std::cout << "Checking authentication\n";
    if(!Auth::handle_authentication(req,res,db)) return;
    std::cout << "Delegating the request to GET home handler\n";
    GetHomeHandler::get_home_handler(req,res,db);
}

void Router::handle_post_home(HttpRequest& req, HttpResponse& res, Database& db){
    std::cout << "Checking authentication\n";
    if(!Auth::handle_authentication(req,res,db)) return;
    // std::cout << "Delegating the request to POST home handler\n";
    // PostHomeHandler::post_home_handler(req,res,db);
    handle_redirect(res,"/");
}

void Router::handle_post_logout(HttpRequest& req, HttpResponse& res, Database& db)
{
    std::cout << "Checking Authentication\n";
    if(!Auth::handle_authentication(req,res,db)) return;
    std::cout << "Delegrating the request to POST logout handler\n";
    PostLogoutHandler::post_logout_handler(req,res,db);
    handle_redirect(res,"/login");
}

void Router::handle_redirect(HttpResponse& res, const std::string& location)
{
    res.set_status(303, "See Other");
    res.set_header("Location",location);
}
