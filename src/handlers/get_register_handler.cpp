#include "get_register_handler.h"

void GetRegisterHandler::get_register_handler(HttpRequest& req, HttpResponse& res, Database& db){
    //OK status 200
    res.set_status(200,"OK");
    //Setting headers
    res.set_header("Content-Type","text/html");         //HTML content
    res.set_header("Cache-Control","no-store");         //Prevent caching

    std::string errorMessage{};
    if(req.has_specific_query_param("error")){
        //Set the error message string acc to query param
        errorMessage = req.get_query_param("error");
    }

    //R(....) indicated that we are going to use the content as it is raw string (no need to escape things like "")
    //name attribute is the key which the request uses to send that input data so name="username" makes sure that form sends username="whateverinput"
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
                                <h1>Registration Page</h1>)";
    
                    if(!errorMessage.empty()){
                        body += R"(<p style="color: red;">)" + errorMessage + R"(</p>)";
                    }

                    body += R"(
                                <label for="user-name">User Name: </label>
                                <input id="user-name" type="text" name="username" placeholder="John Doe" autofocus required>
                                <br><br>
                                <label for="password">Password: </label>
                                <input id="password" name="password" type="password" required>
                                <br><br>
                                <button class="register-button">Register</button>
                            </form>
                        </body>
                        </html>)";
                    

    res.set_body(body);
}   