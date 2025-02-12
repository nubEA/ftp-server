#include "get_history_handler.h"

void GetHistoryHandler::get_history_handler(HttpRequest& req, HttpResponse& res, Database& db)
{
    res.set_status(200, "OK");
    res.set_header("Content-Type","text/html");         //Descibe content as html
    res.set_header("Cache-Control","no-store");         //Prevent caching

    std::vector<std::vector<std::string>> results = get_table_content(get_user_id(req.get_token_cookie(),db),db);

    std::string body = R"(
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Upload History</title>
            <style>
                table { width: 100%; border-collapse: collapse; }
                th, td { border: 1px solid black; padding: 8px; text-align: left; }
                th { background-color: #f2f2f2; }
                .copy-button { cursor: pointer; color: blue; text-decoration: underline; border: none; background: none; }
            </style>
            <script>
                function copyToClipboard(id) {
                    var copyText = document.getElementById(id);
                    copyText.select();
                    copyText.setSelectionRange(0, 99999); // For mobile devices
                    document.execCommand("copy");
                    alert("Copied: " + copyText.value);
                }
            </script>
        </head>
        <body>
            <h1>Upload History</h1>
            <table>
                <tr>
                    <th>Filename</th>
                    <th>File Size</th>
                    <th>Uploaded At</th>
                    <th>Download Link</th>
                    <th>Copy Link</th>
                </tr>)";
        
        // Loop through the results and populate the table
        for (size_t i = 0; i < results.size(); ++i) {
            std::string filename = results[i][1];
            std::string fileSize = results[i][2];
            std::string uploadTimestamp = results[i][3];
            std::string downloadLink = "/download/" + results[i][4]; // Assuming download_link is at index 4
        
            std::string fullLink = "http://localhost:8080" + downloadLink;
            std::string inputId = "copy" + std::to_string(i);
        
            body += "<tr>";
            body += "<td>" + filename + "</td>";
            body += "<td>" + fileSize + "</td>";
            body += "<td>" + uploadTimestamp + "</td>";
            body += "<td><a href='" + fullLink + "' target='_blank'>Download</a></td>";
            body += "<td><input type='text' id='" + inputId + "' value='" + fullLink + "' readonly>";
            body += "<button class='copy-button' onclick='copyToClipboard(\"" + inputId + "\")'>Copy</button></td>";
            body += "</tr>";
        }
        
        body += R"(
            </table>
            <br>
            <a href="/">Back to Home</a>
        </body>
        </html>)";
        
        // Set the response body
        res.set_body(body);
}

std::string GetHistoryHandler::get_user_id(const std::string& token, Database& db)
{
    std::cout << "Fetching User Id\n";
    std::string stmtName = "get_refresh_token";
    const char* params[] = {token.c_str()};
    int paramLen[] = {static_cast<int>(token.length())};
    int paramFormat[] = {0};
    int nParams = 1;
    auto results = db.fetch_results(stmtName,params,paramLen,paramFormat,nParams);
    
    std::cout << "Returning user id\n";

    return results[0][0];
}

std::vector<std::vector<std::string>> GetHistoryHandler::get_table_content(const std::string& userId, Database& db)
{
    std::string stmtName = "file_retrieval_by_id";
    const char* params[] = {userId.c_str()};
    int paramLen[] = {static_cast<int>(userId.length())};
    int paramFormat[] = {0};
    int nParams = 1;

    std::cout << "Makign database query to fetch all files by user\n";    
    std::vector<std::vector<std::string>> results{};
    results = db.fetch_results(stmtName, params, paramLen, paramFormat, nParams);

    return results;
}