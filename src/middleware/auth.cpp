#include "auth.h"

bool Auth::handle_authentication(HttpRequest& req, HttpResponse& res, Database& db){
    if(!req.has_header("Cookie")){
        handle_not_logged_in(req, res, db);
        return false;
    }

    std::string cookie_header = req.get_specific_header("Cookie");
    std::string token = extract_token_from_cookie(cookie_header);
    if (token.empty()) {
        handle_not_logged_in(req, res, db);
        return false;
    }

    std::string hashedToken = bcrypt::generateHash(token);

    std::string stmtName = "get_refresh_token";
    const char* params[] = {hashedToken.c_str()};
    int paramLen[] = {static_cast<int>(hashedToken.length())};
    int paramFormat[] = {0};
    int nParams = 1;

    std::vector<std::vector<std::string>> results = db.fetch_results(stmtName, params, paramLen, paramFormat, nParams);
    
    if (results.empty() || isExpired(results[0][1])) {
        handle_not_logged_in(req, res, db);
        return false;
    }

    return true;
}

void Auth::handle_not_logged_in(HttpRequest& req, HttpResponse& res, Database& db){
    res.set_status(303, "See Other");
    res.set_header("Location", "/login");
}

std::string Auth::extract_token_from_cookie(const std::string& cookie_header) {
    std::string token_prefix = "token=";
    size_t start_pos = cookie_header.find(token_prefix);
    if (start_pos != std::string::npos) {
        start_pos += token_prefix.length();
        size_t end_pos = cookie_header.find(";", start_pos);
        if (end_pos == std::string::npos) {
            end_pos = cookie_header.length();
        }
        return cookie_header.substr(start_pos, end_pos - start_pos);
    }
    return "";
}

bool Auth::isExpired(std::string& expiresAt) {
    std::tm tm = {};
    std::istringstream ss(expiresAt);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

    if (ss.fail()) {
        return true;
    }

    std::time_t time = std::mktime(&tm);
    auto expiration_time = std::chrono::system_clock::from_time_t(time);

    auto now = std::chrono::system_clock::now();
    return expiration_time < now;
}
