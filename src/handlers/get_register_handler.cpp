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
                                    margin-bottom: 30px; /* Increased margin */
                                }

                                form {
                                    background-color: #fff;
                                    padding: 40px; /* Increased padding */
                                    border-radius: 8px;
                                    box-shadow: 0 4px 8px rgba(0,0,0,0.1); /* Stronger shadow */
                                    width: 400px; /* Fixed width for form */
                                    text-align: center;
                                }

                                label {
                                    display: block;
                                    margin-bottom: 10px;
                                    font-weight: bold;
                                    color: #555;
                                    text-align: left; /* Align labels to the left */
                                }

                                input[type="text"],
                                input[type="password"] {
                                    padding: 12px; /* Increased input padding */
                                    margin-bottom: 25px; /* Increased margin */
                                    border: 1px solid #ddd;
                                    border-radius: 4px;
                                    width: calc(100% - 24px); /* Adjust width for padding and border */
                                    font-size: 16px;
                                }

                                input[type="text"]:focus,
                                input[type="password"]:focus {
                                    outline: none;
                                    border-color: #3498db; /* Highlight on focus */
                                    box-shadow: 0 0 5px rgba(52, 152, 219, 0.5); /* Focus shadow */
                                }


                                p.error-message {
                                    color: #c0392b; /* Darker red for error */
                                    margin-bottom: 20px;
                                    background-color: #fdedec; /* Light red background for error box */
                                    padding: 15px;
                                    border-radius: 4px;
                                    border: 1px solid #e74c3c; /* Slightly darker red border */
                                    text-align: center;
                                }


                                button.register-button {
                                    background-color: #2ecc71; /* Green register button */
                                    color: white;
                                    padding: 14px 25px; /* Increased button padding */
                                    border: none;
                                    border-radius: 4px;
                                    cursor: pointer;
                                    font-size: 18px; /* Larger font size */
                                    transition: background-color 0.3s ease;
                                    margin-top: 10px; /* Add some top margin */
                                }

                                button.register-button:hover {
                                    background-color: #27ae60; /* Darker green on hover */
                                }
                            </style>
                        </head>
                        <body>
                            <form method="POST" action="/register">
                                <h1>Create Account</h1>)";
    
                    if(!errorMessage.empty()){
                        body += R"(<p class="error-message">)" + errorMessage + R"(</p>)";
                    }

                    body += R"(
                                <label for="user-name">Username</label>
                                <input id="user-name" type="text" name="username" placeholder="Enter a username" autofocus required>
                                
                                <label for="password">Password</label>
                                <input id="password" name="password" type="password" placeholder="Enter a password" required>
                                
                                <button class="register-button" type="submit">Register</button>
                            </form>
                        </body>
                        </html>)";
                    

    res.set_body(body);
}