#include "get_login_handler.h"

void GetLoginHandler::get_login_handler(HttpRequest& req, HttpResponse& res, Database& db){
    res.set_status(200,"OK");
    res.set_header("Content-Type","text/html");         //HTML content
    res.set_header("Cache-Control","no-store");         //Prevent caching

    std::string userCreated{};
    //Extracting query parameter in case redirected from /register to /login
    if(req.has_specific_query_param("userCreated")){
        userCreated = "Account created successfully! Please log in.";
    }
    std::string errorMessage{};
    if(req.has_specific_query_param("error")){
        errorMessage = req.get_query_param("error");
    }


    ///Sending the HTML page as response body
    std::string body =   R"(<!DOCTYPE html>
                            <html lang="en">
                            <head>
                                <meta charset="UTF-8">
                                <meta name="viewport" content="width=device-width, initial-scale=1.0">
                                <title>Login</title>
                                <style>
                                    body {
                                        font-family: sans-serif;
                                        color: #333;
                                        margin: 0;
                                        padding: 20px;
                                        background-color: #f4f4f4;
                                        display: flex;
                                        flex-direction: column;
                                        align-items: center;
                                    }

                                    h1 {
                                        color: #3498db;
                                        text-align: center;
                                        margin-bottom: 30px;
                                    }

                                    form {
                                        background-color: #fff;
                                        padding: 40px;
                                        border-radius: 8px;
                                        box-shadow: 0 4px 8px rgba(0,0,0,0.1);
                                        width: 400px;
                                        text-align: center;
                                    }

                                    label {
                                        display: block;
                                        margin-bottom: 10px;
                                        font-weight: bold;
                                        color: #555;
                                        text-align: left;
                                    }

                                    input[type="text"],
                                    input[type="password"] {
                                        padding: 12px;
                                        margin-bottom: 25px;
                                        border: 1px solid #ddd;
                                        border-radius: 4px;
                                        width: calc(100% - 24px);
                                        font-size: 16px;
                                    }

                                    input[type="text"]:focus,
                                    input[type="password"]:focus {
                                        outline: none;
                                        border-color: #3498db;
                                        box-shadow: 0 0 5px rgba(52, 152, 219, 0.5);
                                    }

                                    p.success-message {
                                        color: green;
                                        margin-bottom: 20px;
                                        text-align: center;
                                    }

                                    p.error-message {
                                        color: #c0392b;
                                        margin-bottom: 20px;
                                        background-color: #fdedec;
                                        padding: 15px;
                                        border-radius: 4px;
                                        border: 1px solid #e74c3c;
                                        text-align: center;
                                    }


                                    button.login-button {
                                        background-color: #3498db; /* Blue login button */
                                        color: white;
                                        padding: 14px 25px;
                                        border: none;
                                        border-radius: 4px;
                                        cursor: pointer;
                                        font-size: 18px;
                                        transition: background-color 0.3s ease;
                                        margin-top: 10px;
                                    }

                                    button.login-button:hover {
                                        background-color: #2980b9; /* Darker blue on hover */
                                    }

                                    .register-link {
                                        display: inline-block;
                                        margin-top: 20px;
                                        color: #3498db;
                                        text-decoration: none;
                                    }

                                    .register-link:hover {
                                        text-decoration: underline;
                                    }
                                </style>
                            </head>
                            <body>
                                <form method="POST" action="/login">
                                    <h1>Sign In</h1>)";

    if(!userCreated.empty()) body += R"(<p class="success-message">)" + userCreated + R"(</p>)";
    if(!errorMessage.empty()) body += R"(<p class="error-message">)" + errorMessage + R"(</p>)";


                            body += R"(
                                    <label for="user-name">Username</label>
                                    <input id="user-name" type="text" name="username" placeholder="Enter your username" autofocus required>
                                    
                                    <label for="password">Password</label>
                                    <input id="password" type="password" name="password" placeholder="Enter your password" required>
                                    
                                    <button class="login-button" type="submit">Login</button>
                                    <a href="/register" class="register-link">Create an account</a> 
                                </form>
                            </body>
                            </html>)";

    res.set_body(body);
}