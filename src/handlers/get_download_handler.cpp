#include "get_download_handler.h"
#define MAX_CHUNK 64*1024

void GetDownloadHandler::get_download_handler(HttpRequest& req, HttpResponse& res, Database& db, int fd){
    std::cout << "[INFO] Handling download request\n";
    std::string path = req.get_path();
    size_t downloadPos = path.find("download/");
    if (downloadPos == std::string::npos) {
        std::cerr << "[ERROR] Invalid download path\n";
        handle_file_not_found(req, res, db);
        return;
    }

    std::string hashedFileName = path.substr(downloadPos + std::string("download/").length());
    std::cout << "[INFO] Extracted hashed file name: " << hashedFileName << "\n";

    struct GetDownloadHandler::fileInfo info = fetch_file_from_link(req, res, db, hashedFileName);

    if (!res.get_body().empty()) {
        std::cerr << "[INFO] Response already set, stopping handler\n";
        return;
    }

    std::cout << "[INFO] Retrieved file info - User ID: " << info.userId 
              << ", Filename: " << info.filename 
              << ", Filesize: " << info.filesize 
              << ", Permissions: " << info.perms << "\n";

    if(info.perms == "private") {
        std::cout << "[INFO] File is private, checking authentication\n";
        if(Auth::handle_authentication(req, res, db)) {
            std::string token = req.get_token_cookie();
            std::string user = Auth::get_user_id(token, db);
            if(user != info.userId) {
                std::cerr << "[WARNING] Unauthorized access attempt by user: " << user << "\n";
                handle_perm_failure(req, res, db);
                return;
            }
        } else {
            std::cerr << "[ERROR] Authentication failed\n";
            return;
        }
    }
    
    std::cout << "[INFO] Sending file to client\n";
    handle_success(req, res, db, hashedFileName, info, fd);
}


void GetDownloadHandler::handle_perm_failure(HttpRequest& req, HttpResponse& res, Database& db)
{
    res.set_status(403, "Forbidden");
    res.set_header("Content-Type","text/html");
    res.set_header("Cache-Control","no-store");        

    std::string body = 
        R"(<!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>403 Forbidden</title>
            <style>
                body {
                    font-family: Arial, sans-serif;
                    text-align: center;
                    background-color: #f8f9fa;
                    padding: 50px;
                }
                .container {
                    max-width: 600px;
                    margin: auto;
                    background: white;
                    padding: 20px;
                    border-radius: 8px;
                    box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
                }
                h1 {
                    color: #dc3545;
                }
                p {
                    color: #333;
                }
            </style>
        </head>
        <body>
            <div class="container">
                <h1>403 Forbidden</h1>
                <p>You do not have permission to access this file.</p>
            </div>
        </body>
        </html>)";
    
    res.set_body(body);
}

void GetDownloadHandler::handle_file_not_found(HttpRequest& req, HttpResponse& res, Database& db)
{
    res.set_status(404, "Not Found");
    res.set_header("Content-Type", "text/html");
    res.set_header("Cache-Control", "no-store");

    std::string body =
        R"(<!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>404 Not Found</title>
            <style>
                body {
                    font-family: Arial, sans-serif;
                    text-align: center;
                    background-color: #f8f9fa;
                    padding: 50px;
                }
                .container {
                    max-width: 600px;
                    margin: auto;
                    background: white;
                    padding: 20px;
                    border-radius: 8px;
                    box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
                }
                h1 {
                    color: #dc3545;
                }
                p {
                    color: #333;
                }
            </style>
        </head>
        <body>
            <div class="container">
                <h1>404 Not Found</h1>
                <p>The file you are looking for does not exist or has been removed.</p>
            </div>
        </body>
        </html>)";

    res.set_body(body);
}


void GetDownloadHandler::handle_success(HttpRequest& req, HttpResponse& res, Database& db, const std::string& hashedName, GetDownloadHandler::fileInfo& info, int client_socket){
    std::string filePath = "/home/harshit/code/http-server/uploads/" + hashedName;
    filePath = Util::trim(filePath);

    std::cout << "[DEBUG] Preparing to send file: " << filePath << "\n";

    if(!std::filesystem::exists(filePath))
    {
        std::cerr << "[ERROR] File not found: " << filePath << "\n";
        handle_file_not_found(req,res,db);
        return;
    }

    std::cout << "[DEBUG] Opening file for reading\n";
    std::ifstream filestream(filePath, std::ios::binary);

    if (!filestream) {
        std::cerr << "[ERROR] Failed to open file: " << filePath << std::endl;
        handle_file_not_found(req, res, db);
        return;
    }
    std::cout << "[INFO] File opened successfully: " << filePath << std::endl;


    res.set_status(200, "OK");
    res.set_header("Content-Type", "application/octet-stream");
    res.set_header("Content-Disposition", "attachment; filename=\"" + info.filename + "\"");
    res.set_header("Cache-Control", "no-store");
    res.set_header("Transfer-Encoding", "chunked");

    std::string responseHeaders = res.get_string_response();
    std::cout << "[DEBUG] Sending headers to client\n";
    send(client_socket, responseHeaders.c_str(), responseHeaders.size(), 0);

    std::vector<unsigned char> buff(MAX_CHUNK);

    while (true) 
    {
        filestream.read(reinterpret_cast<char*>(buff.data()), MAX_CHUNK);
        size_t bytesRead = filestream.gcount();

        if (bytesRead == 0) {
            std::cout << "[DEBUG] End of file reached\n";
            break; // No more data, exit loop
        }

        // Convert bytesRead to HEX string
        std::ostringstream chunkSizeStream;
        chunkSizeStream << std::hex << bytesRead;
        std::string chunkSize = chunkSizeStream.str() + "\r\n";

        // Send chunk size
        send(client_socket, chunkSize.c_str(), chunkSize.size(), 0);
        std::cout << "[DEBUG] Sent chunk size: " << chunkSize << "\n";

        // Send actual chunk data
        send(client_socket, reinterpret_cast<char*>(buff.data()), bytesRead, 0);
        std::cout << "[DEBUG] Sent chunk of " << bytesRead << " bytes\n";

        // Send CRLF after chunk data
        send(client_socket, "\r\n", 2, 0);
    }

    // Send final empty chunk to signal end of response
    std::cout << "[INFO] Sending final empty chunk\n";
    send(client_socket, "0\r\n\r\n", 5, 0);
}


GetDownloadHandler::fileInfo GetDownloadHandler::fetch_file_from_link(HttpRequest& req, HttpResponse& res, Database& db, const std::string& link)
{
    std::cout << "[DEBUG] Querying database for file: " << link << "\n";

    std::string stmtName = "file_retrieval";
    const char* params[] = {link.c_str()};
    int paramLen[] = {static_cast<int>(link.length())};
    int paramFormat[] = {0};
    int nParams = 1;

    std::vector<std::vector<std::string>> results = db.fetch_results(stmtName,params,paramLen,paramFormat,nParams);

    struct GetDownloadHandler::fileInfo info{};

    if (results.empty() || results[0].size() < 7) {
        std::cerr << "[ERROR] No results from database for file: " << link << "\n";
        handle_file_not_found(req,res,db);
        return info;
    }    

    info.userId = results[0][0];
    info.filename = results[0][1];
    info.filesize = results[0][2];
    info.perms = results[0][6];

    std::cout << "[DEBUG] Retrieved file info from DB: User ID = " << info.userId
              << ", Filename = " << info.filename
              << ", Filesize = " << info.filesize
              << ", Permissions = " << info.perms << "\n";

    return info;
}
