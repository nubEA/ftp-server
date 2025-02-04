#include "post_register_handler.h"

void PostRegisterHandler::post_register_handler(HttpRequest& req, HttpResponse& res, Database& db) {
    std::string body = req.get_body();
    // Get username and password from body
    User user = get_credentials(body, res); // Pass res

    if (!handle_validation(user.username, user.password, db, res)) return; // Pass res
    if (!create_user(user.username, user.password, db, res)) return; // Pass res
}

bool PostRegisterHandler::is_password_valid(const std::string& password) {
    // At least 8 char long and at max 64 chars long
    if (password.length() < 8 || password.length() > 64) return false;
    return Util::has_upper_case(password) && Util::has_lower_case(password) && Util::has_digit(password);
}

void PostRegisterHandler::handle_invalid_credential_error(HttpResponse& res) {
    std::string errorMessage = "Invalid username or password, make sure: username contains alphanumeric and underscores only in range [4,20] and passwords have at least 1 upper case, 1 lower case, 1 digit and min length of 8 chars and max 64 chars.";

    // Redirect to the /register page
    res.set_status(303, "See Other");
    res.set_header("Location", "/register?error=" + Util::url_encode(errorMessage));
}

void PostRegisterHandler::handle_success(HttpResponse& res) {
    res.set_status(303, "See Other");
    res.set_header("Location", "/login?userCreated=true");
}

PostRegisterHandler::User PostRegisterHandler::get_credentials(const std::string& body, HttpResponse& res) {
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

    std::cout << "Username and password extracted from the body\n";

    return user;
}

bool PostRegisterHandler::handle_validation(const std::string& username, const std::string& password, Database& db, HttpResponse& res) {
    // Passing the parameters in this array as C-style string
    const char* params[] = {username.c_str()};
    // Passing the length of each param
    int paramLen[] = {static_cast<int>(username.length())};
    // Passing the format 0 -> Text, 1 -> binary
    int paramFormat[] = {0};
    // Number of parameters
    int nParams = 1;
    std::vector<std::vector<std::string>> result = db.fetch_results("user_name_available", params, paramLen, paramFormat, nParams);
    std::string count = result[0][0];
    int countOfnames;

    try {
        countOfnames = std::stoi(count);
    } catch (const std::exception& e) {
        std::cout << "Error in validating if username exists or not\n";
        Util::make_error_response(400, "Bad Request", res);
        return false;
    }

    // If countOfnames == 0 then no such names exist in the database
    bool isUserNameValid = Util::is_username_valid(username) && (countOfnames == 0);
    bool isPassValid = is_password_valid(password);

    // Log whether username is invalid or password
    if (!isUserNameValid) std::cout << "Username not valid\n";
    if (!isPassValid) std::cout << "Password not valid\n";

    if (!isUserNameValid || !isPassValid) {
        handle_invalid_credential_error(res);
        return false;
    }
    return true;
}

bool PostRegisterHandler::create_user(const std::string& username, const std::string& password, Database& db, HttpResponse& res) {
    std::cout << "Passed all validations, trying to create the user now...\n";
    std::string hashedPassword = bcrypt::generateHash(password);
    // Filling in necessary arguments for psql query
    std::string stmtName = "user_reg";
    const char* params[] = {username.c_str(), hashedPassword.c_str()};
    int paramLen[] = {static_cast<int>(username.length()), static_cast<int>(hashedPassword.length())};
    int paramFormat[] = {0, 0};
    int nParams = 2;

    bool success = db.execute_query(stmtName, params, paramLen, paramFormat, nParams);

    if (success) {
        std::cout << "User created successfully\n";
        handle_success(res);
        return true;
    } else {
        std::cout << "Error in creating user\n";
        Util::make_error_response(500, "Internal Server Error", res);
        return false;
    }
}