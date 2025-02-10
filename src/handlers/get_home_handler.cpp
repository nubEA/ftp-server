#include "get_home_handler.h"

void GetHomeHandler::get_home_handler(HttpRequest& req, HttpResponse& res, Database& db){
    res.set_status(200,"OK");
    res.set_header("Content-Type","text/html");         //Descibe content as html
    res.set_header("Cache-Control","no-store");         //Prevent caching
  
std::string body = R"(<!DOCTYPE html>
                        <html lang="en">
                        <head>
                            <meta charset="UTF-8">
                            <meta name="viewport" content="width=device-width, initial-scale=1.0">
                            <title>File Transfer</title>
                        </head>
                        <body>
                            <h1>File Transfer!!</h1>
                            <form method="POST" action="/" enctype="multipart/form-data">
                                <label for="file-upload">Insert File </label>
                                <br><br>
                                <input id="file-upload" type="file" name="file" accept=".txt, .jpg, .png, .pdf, .zip, .mp3" required>
                                <br><br>
                                <input type="checkbox" id="public-checkbox" name="public">
                                <label for="public-checkbox">Make Public</label>
                                <br><br>
                                <button type="submit" value="upload">Upload File</button>
                            </form>
                            <br><br>
                            <form method="POST" action="/logout">
                                <button type="submit" value="logout">Log Out</button>
                            </form>
                        </body>
                        </html>)";


    res.set_body(body);
}
