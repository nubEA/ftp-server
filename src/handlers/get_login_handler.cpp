#include "get_login_handler.h"

void GetLoginHandler::get_login_handler(HttpRequest& req, HttpResponse& res, Database& db){
    res.set_status(200,"OK");
    res.set_header("Content-Type","text/html");         //HTML content
    res.set_header("Cache-Control","no-store");         //Prevent caching

    std::string userCreated{};
    //Extracting query parameter in case redirected from /register to /login
    if(req.has_specific_query_param("userCreated")){
        userCreated = "User created successfully. Login";
    }

    ///Sending the HTML page as response body
    std::string body =   R"(<!DOCTYPE html>
                            <html lang="en">
                            <head>
                                <meta charset="UTF-8">
                                <meta name="viewport" content="width=device-width, initial-scale=1.0">
                                <title>Register</title>
                            </head>
                            <body>)";

    if(!userCreated.empty()) body += R"(<p style="color: green;">)" + userCreated + R"(</p>)";

                            body += R"(<form method="POST" action="/login">
                                    <h1>Login Page</h1>
                                    <label for="user-name">User Name: </label>
                                    <input id="user-name" type="text" name="username" placeholder="John Doe" autofocus required>
                                    <br><br>
                                    <label for="password">Password: </label>
                                    <input id="password" type="password" name="password" required>
                                    <br><br>
                                    <button class="login-button">Log in</button>
                                </form>
                            </body>
                            </html>)";

    res.set_body(body);
}
