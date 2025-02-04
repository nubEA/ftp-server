#include "post_register_handler.h"

bool PostRegisterHandler::is_password_valid(std::string& password)
{
    //Atleast 8 char long and at max 64 chars long
    if(password.length() < 8 || password.length() > 64) return false;
    return Util::has_upper_case(password) && Util::has_lower_case(password) && Util::has_digit(password);    
}

void PostRegisterHandler::handle_invalid_credential_error(HttpResponse& res)
{   
    std::string errorMessage = "Invalid username or password, make sure: username contains alphanumeric and underscores only in range [4,20] and passwords have atleast 1 upper case, 1 lower case, 1 digit and min length of 8 chars and max 64 chars.";

    //Redirect to the /register page
    res.set_status(303, "See Other");
    res.set_header("Location", "/register?error=" + Util::url_encode(errorMessage));
    return;
}

void PostRegisterHandler::post_register_handler(HttpRequest& req, HttpResponse& res, Database& db){

    std::string body = req.get_body();
    //username=<username>&password=<password> so using this template to extract username and pass from body
    size_t username_pos = body.find("username=");
    size_t seperator_pos = body.find("&");
    size_t password_pos = body.find("password=");
    //adding the offset
    size_t user_start = username_pos+std::string("username=").length();
    size_t pass_start = password_pos+std::string("password=").length();
    
    //If no & then the request is malformed
    if(seperator_pos == std::string::npos || username_pos == std::string::npos || password_pos == std::string::npos)
    {
        Util::make_error_response(400, "Bad Request", res);
        return;   
    }
    //Extracting username and password
    std::string username = body.substr(user_start,seperator_pos-user_start+1);
    std::string password = body.substr(pass_start);
    
    std::cout << "Username and password extracted from the body\n";

    //Passing the parameters in this array as C-style string
    const char* params[] = {username.c_str()};
    //Passing the length of each param
    int paramLen[] = {static_cast<int>(username.length())};
    //Passing the format 0 -> Text, 1 -> binary
    int paramFormat[] = {0};
    //Number of parameters
    int nParams = 1;
    std::vector<std::vector<std::string>> result = db.fetch_results("user_name_available",params,paramLen,paramFormat,nParams);
    std::string count = result[0][0];
    int countOfnames;
    try
    {
        countOfnames = std::stoi(count);
    }
    catch(const std::exception& e)
    {
        Util::make_error_response(400, "Bad Request", res);
        return;
    }
    
    bool isUserNameValid = Util::is_username_valid(username) && (countOfnames == 0);
    bool isPassValid = is_password_valid(password);

    if(!isUserNameValid || !isPassValid) 
    {
        handle_invalid_credential_error(res);
    }
    else{

    }
}