#include "post_login_handler.h"

void PostLoginHandler::post_login_handler(HttpRequest& req, HttpResponse& res, Database& db)
{
    std::string body = req.get_body();
    struct User user = get_credentials(body,res);
    std::string id{};
    //Exit if validation fails
    if(!handle_validation(user.username,user.password,db,res,id)) return;
    handle_session_token(user.username,db,res,id);
    handle_success(res);
}

void PostLoginHandler::handle_success(HttpResponse& res)
{
    res.set_status(303,"See Other");
    res.set_header("Location", "/");
}

void PostLoginHandler::handle_session_token(std::string& username, Database& db, HttpResponse& res, const std::string& id){
    unsigned char buff[32];
    // Create random 32 bytes of data (stored in buff) to be used as refresh token for validation
    RAND_bytes(buff, 32);
    std::string token{};
    std::stringstream ss;
    for(auto& v : buff) {
        // buff is filled but '0' and '1's since RAND just fills it with random byte string
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)(v);
    }
    token = ss.str();

    // std::chrono::system_clock::now() returns a std::time_point so we convert it to time_t
    // std::time_t represents time as the number of seconds elapsed since a fixed point
    // std::put_time used to convert time_t (std::tm) to some time format like yyyy:mm:dd etc
    // std::localtime converts time_t to std::tm
    std::time_t expiresAtTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now() + std::chrono::days(15));
    //Clearing the string stream
    ss.str(std::string());
    ss << std::put_time(std::localtime(&expiresAtTime), "%Y-%m-%d %H:%M:%S");
    std::string expiresAt = ss.str();

    // Set the cookie with the generated token and expiration time
    set_cookie(token, expiresAt, res);

    // Populating all the params needed to execute the query
    std::string stmtName = "insert_refresh_token";
    const char* params[] = {id.c_str(), token.c_str(), expiresAt.c_str()};
    int paramLen[] = {static_cast<int>(id.length()), static_cast<int>(token.length()), static_cast<int>(expiresAt.length())};
    int paramFormat[] = {0, 0, 0};
    int nParams = 3;

    if (db.execute_query(stmtName, params, paramLen, paramFormat, nParams)) {
        std::cout << "Refresh token inserted in the database successfully!\n";
    } else {
        std::cout << "Error in inserting refresh token in database\n";
        std::string message = "Error in inserting refresh token in database";
        handle_invalid_credential_error(res, message);
    }
}


void PostLoginHandler::set_cookie(const std::string& token, const std::string& expiresAt, HttpResponse& res) {
    // AI doing the conversion for me
    std::tm tm = {};
    std::istringstream ss(expiresAt);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    std::time_t expiresAtTime = std::mktime(&tm);

    // Convert to UTC time
    std::tm* gmt_tm = std::gmtime(&expiresAtTime);

    // Format the expiresAt in Cookie format: Mon, 14 Feb 2025 12:00:00 GMT
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt_tm);
    std::string expiresAtCookieFormat = buffer;

    // not using secure as that makes the cookie only be sent over https but my localhost will be http :(
    // cookie contains: HttpOnly -> makes it not accessible via JS good for security
    // Path=/ tells the browser to send this cookie to any request on this localhost/ path
    // SameSite=Strict makes it so that now cookie will only be sent in requests that are originating from the same domain as its creator
    // Expires time should be in the following format: Mon, 14 Feb 2025 12:00:00 GMT
    std::string cookie = "token=" + token + "; Expires=" + expiresAtCookieFormat + "; HttpOnly; SameSite=Strict; Path=/";
    res.set_header("Set-Cookie", cookie);
    std::cout << "Cookie header set successfully\n";
}


PostLoginHandler::User PostLoginHandler::get_credentials(const std::string& body, HttpResponse& res) {
    // username=<username>&password=<password> so using this template to extract username and pass from body
    size_t username_pos = body.find("username=");
    size_t seperator_pos = body.find("&");
    size_t password_pos = body.find("password=");
    // Adding the offset
    size_t user_start = username_pos + std::string("username=").length();
    size_t pass_start = password_pos + std::string("password=").length();

    // If no & then the request is malformed
    if (seperator_pos == std::string::npos || username_pos == std::string::npos || password_pos == std::string::npos) {
        std::cout << "Error in finding separator, username, or password\n";
        Util::make_error_response(400, "Bad Request", res);
        return User{}; // Return an empty User struct
    }

    User user;
    // Extracting username and password
    user.username = body.substr(user_start, seperator_pos - user_start);
    user.password = body.substr(pass_start);
    user.username = Util::url_decode(user.username);
    user.password = Util::url_decode(user.password);

    std::cout << "Username and password extracted from the body\n";

    return user;
}

bool PostLoginHandler::handle_validation(const std::string& username, const std::string& password, Database& db, HttpResponse& res, std::string& id) {
    // Passing the parameters in this array as C-style string
    const char* params[] = {username.c_str()};
    // Passing the length of each param
    int paramLen[] = {static_cast<int>(username.length())};
    // Passing the format 0 -> Text, 1 -> binary
    int paramFormat[] = {0};
    // Number of parameters
    int nParams = 1;
    std::vector<std::vector<std::string>> result = db.fetch_results("user_login", params, paramLen, paramFormat, nParams);

    //Handles the case where user does not exist
    if(result.empty()){
        std::cout << "User does not exist\n";
        std::string message = "User does not exist";
        handle_invalid_credential_error(res,message);
        return false;
    }

    //Matrix of id and password
    id = result[0][0];
    std::string passInDB = result[0][1];

    //Using bcrypt to validate password
    bool isPassValid = bcrypt::validatePassword(password,passInDB);

    if (!isPassValid){
        std::cout << "Password does not match\n";
        std::string message = "Password does not match\n";
        handle_invalid_credential_error(res, message);
        return false;
    }
    else{
        std::cout << "Password Matched!\n";
    }
    return true;
}

void PostLoginHandler::handle_invalid_credential_error(HttpResponse& res, const std::string& message){
    std::string errorMessage = message;

    // Redirect to the /register page
    res.set_status(303, "See Other");
    res.set_header("Location", "/login?error=" + Util::url_encode(errorMessage));
}