#include "post_logout_handler.h"

void PostLogoutHandler::post_logout_handler(HttpRequest& req, HttpResponse& res, Database& db){
    //Get the cookie
    std::string token = req.get_token_cookie();
    remove_cookie_db_entry(get_user_id(token,db),db);
}

std::string PostLogoutHandler::get_user_id(const std::string& token, Database& db)
{
    std::string stmtName = "get_refresh_token";
    const char* params[] = {token.c_str()};
    int paramLen[] = {static_cast<int>(token.length())};
    int paramFormat[] = {0};
    int nParams = 1;
    auto results = db.fetch_results(stmtName,params,paramLen,paramFormat,nParams);
    
    std::cout << "Returning user id\n";

    return results[0][0];
}

void PostLogoutHandler::remove_cookie_db_entry(const std::string& userId, Database& db){
    std::string stmtName = "delete_all_tokens_for_user";
    const char* params[] = {userId.c_str()};
    int paramLen[] = {static_cast<int>(userId.length())};
    int paramFormat[] = {0};
    int nParams = 1;
    if(db.execute_query(stmtName,params,paramLen,paramFormat,nParams)){
        std::cout << "User tokens removed successfully\n";
    }
    else{
        std::cout << "Error occurred during deleting tokens from db\n";
    }
}
