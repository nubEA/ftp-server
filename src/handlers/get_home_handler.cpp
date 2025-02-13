#include "get_home_handler.h"

void GetHomeHandler::get_home_handler(HttpRequest& req, HttpResponse& res, Database& db){
    res.set_status(200,"OK");
    res.set_header("Content-Type","text/html");         //Descibe content as html
    res.set_header("Cache-Control","no-store");         //Prevent caching

    std::string linkQueryParam{};
    if(req.has_specific_query_param("link"))
    {
        linkQueryParam = req.get_query_param("link");
    }

std::string body = R"(<!DOCTYPE html>
                        <html lang="en">
                        <head>
                            <meta charset="UTF-8">
                            <meta name="viewport" content="width=device-width, initial-scale=1.0">
                            <title>File Transfer</title>
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
                                    margin-bottom: 20px;
                                }

                                p {
                                    text-align: center;
                                    margin-bottom: 15px;
                                }

                                .success-message {
                                    color: green;
                                    margin-bottom: 20px;
                                }

                                .upload-form-container, .history-logout-container {
                                    background-color: #fff;
                                    padding: 30px;
                                    border-radius: 8px;
                                    box-shadow: 0 2px 5px rgba(0,0,0,0.1);
                                    margin-bottom: 20px;
                                    width: 400px; /* Adjust width as needed */
                                    text-align: center;
                                }

                                .upload-form-container h2,  .history-logout-container h2{
                                    color: #3498db;
                                    margin-top: 0;
                                    margin-bottom: 20px;
                                }


                                label {
                                    display: block;
                                    margin-bottom: 10px;
                                    font-weight: bold;
                                    color: #555;
                                }

                                input[type="file"] {
                                    margin-bottom: 20px;
                                    padding: 10px;
                                    border: 1px solid #ddd;
                                    border-radius: 4px;
                                    width: calc(100% - 22px); /* Adjust width to account for padding and border */
                                }

                                input[type="checkbox"] {
                                    margin-right: 5px;
                                    vertical-align: middle;
                                }

                                .checkbox-label {
                                    display: inline-block;
                                    vertical-align: middle;
                                    margin-bottom: 20px;
                                    font-weight: normal;
                                    color: #555;
                                }


                                button[type="submit"] {
                                    background-color: #3498db;
                                    color: white;
                                    padding: 12px 20px;
                                    border: none;
                                    border-radius: 4px;
                                    cursor: pointer;
                                    font-size: 16px;
                                    transition: background-color 0.3s ease;
                                }

                                button[type="submit"]:hover {
                                    background-color: #2980b9;
                                }

                                a {
                                    color: #3498db;
                                    text-decoration: none;
                                    transition: color 0.3s ease;
                                }

                                a:hover {
                                    color: #2980b9;
                                    text-decoration: underline;
                                }
                            </style>
                        </head>
                        <body>
                            <h1>File Transfer!!</h1>)";

if(!linkQueryParam.empty()) body += R"(<p class="success-message">File uploaded successfully: <a href=")" + linkQueryParam + R"(">Download Link</a> </p>)";

body +=                         R"(<div class="upload-form-container">
                                <h2>Upload File</h2>
                                <form method="POST" action="/" enctype="multipart/form-data">
                                    <input type="checkbox" id="public-checkbox" name="public" value="on">
                                    <label for="public-checkbox" class="checkbox-label">Make Public</label>
                                    <br>
                                    <label for="file-upload">Choose File</label>
                                    <input id="file-upload" type="file" name="file" accept=".txt, .jpg, .png, .pdf, .zip, .mp3" required>
                                    <button type="submit" value="upload">Upload File</button>
                                </form>
                            </div>

                            <div class="history-logout-container">
                                <h2>Navigation</h2>
                                <p><a href="/history">View Upload History</a></p>
                                <form method="POST" action="/logout">
                                    <button type="submit" value="logout">Log Out</button>
                                </form>
                            </div>
                        </body>
                        </html>)";


    res.set_body(body);
}