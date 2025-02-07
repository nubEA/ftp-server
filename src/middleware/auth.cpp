#include "auth.h"

bool Auth::handle_authentication(HttpRequest& req, HttpResponse& res, Database& db){
    if(!req.has_header("cookie")){
        std::cout << "No cookie header found\n";
        handle_not_logged_in(req, res, db);
        return false;
    }
    std::string token = req.get_token_cookie();
    std::cout << "Extracted token from cookie: " << token << '\n';

    if (token.empty()) {
        std::cout << "Token is empty\n";
        handle_not_logged_in(req, res, db);
        return false;
    }

    std::string stmtName = "get_refresh_token";
    const char* params[] = {token.c_str()};
    int paramLen[] = {static_cast<int>(token.length())};
    int paramFormat[] = {0};
    int nParams = 1;

    std::vector<std::vector<std::string>> results = db.fetch_results(stmtName, params, paramLen, paramFormat, nParams);

    if (results.empty()) {
        std::cout << "No results found for the token\n";
        handle_not_logged_in(req, res, db);
        return false;
    }

    if (isExpired(results[0][1], token, db)) {
        std::cout << "Token is expired\n";
        handle_not_logged_in(req, res, db);
        return false;
    }

    return true;
}

void Auth::handle_not_logged_in(HttpRequest& req, HttpResponse& res, Database& db){
    res.set_status(303, "See Other");
    res.set_header("Location", "/login");
}

std::string Auth::get_user_id(const std::string& token, Database& db)
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

void Auth::remove_cookie_db_entry(const std::string& userId, Database& db){
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

bool Auth::isExpired(std::string& expiresAt, std::string& token, Database& db) {
    std::tm tm = {};
    std::istringstream ss(expiresAt);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

    if (ss.fail()) {
        return true;
    }

    std::time_t time = std::mktime(&tm);
    auto expiration_time = std::chrono::system_clock::from_time_t(time);

    auto now = std::chrono::system_clock::now();

    if(expiration_time < now)
    {
        std::cout << "Cookie has expired\n";
        remove_cookie_db_entry(get_user_id(token,db),db);
    }

    return expiration_time < now;
}
