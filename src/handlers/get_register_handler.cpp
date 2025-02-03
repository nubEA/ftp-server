#include "get_register_handler.h"

void GetRegisterHandler::get_register_handler(HttpRequest& req, HttpResponse& res){
    //OK status 200
    res.set_status(200,"OK");
    //Setting headers
    res.set_header("Content-Type","text/html");         //HTML content
    res.set_header("Cache-Control","no-store");         //Prevent caching

    //R(....) indicated that we are going to use the content as it is raw string (no need to escape things like "")
    std::string body = 
                        R"(<!DOCTYPE html>
                        <html lang="en">
                        <head>
                            <meta charset="UTF-8">
                            <meta name="viewport" content="width=device-width, initial-scale=1.0">
                            <title>Register</title>
                        </head>
                        <body>
                            <form method="POST" action="/register">
                                <h1>Registration Page</h1>
                                <label for="user-name">User Name: </label>
                                <input id="user-name" type="text" placeholder="John Doe" autofocus required>
                                <br><br>
                                <label for="password">Password: </label>
                                <input id="password" type="password" required>
                                <br><br>
                                <button class="register-button">Register</button>
                            </form>
                        </body>
                        </html>)";
                    

    res.set_body(body);
}   