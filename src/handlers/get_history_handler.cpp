#include "get_history_handler.h"

void GetHistoryHandler::get_history_handler(HttpRequest& req, HttpResponse& res, Database& db)
{
    res.set_status(200, "OK");
    res.set_header("Content-Type","text/html");
    res.set_header("Cache-Control","no-store");

    std::vector<std::vector<std::string>> results = get_table_content(get_user_id(req.get_token_cookie(),db),db);
    std::reverse(results.begin(),results.end());

    std::string body = R"(
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Upload History</title>
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

                table { 
                    width: 80%; /* Wider table for better readability */
                    border-collapse: collapse; 
                    margin-bottom: 20px;
                    background-color: white; /* White background for table */
                    box-shadow: 0 2px 5px rgba(0,0,0,0.1); /* Subtle shadow */
                    border-radius: 8px; /* Rounded corners for table */
                    overflow: hidden; /* Ensure rounded corners are respected */
                }

                th, td { 
                    border-bottom: 1px solid #ddd; /* Lighter horizontal lines */
                    padding: 12px 15px; /* More padding for comfortable reading */
                    text-align: left; 
                }

                th { 
                    background-color: #3498db; /* Blue header */
                    color: white; 
                    font-weight: bold;
                    text-transform: uppercase; /* Uppercase for headers */
                    letter-spacing: 1px; /* Spacing for headers */
                }

                /* Zebra striping for rows */
                tbody tr:nth-child(even) {
                    background-color: #f9f9f9;
                }

                tbody tr:hover {
                    background-color: #f2f2f2; /* Slightly highlight on hover */
                }


                td:last-child {
                    text-align: center; /* Center align the last column (Copy Link) */
                }


                .copy-button { 
                    cursor: pointer; 
                    color: #3498db; /* Blue copy button text */
                    text-decoration: none; /* Remove underline */
                    border: 1px solid #3498db; /* Blue border */
                    background-color: white; /* White button background */
                    padding: 8px 15px;
                    border-radius: 4px;
                    transition: background-color 0.3s, color 0.3s;
                    font-size: 0.9em;
                }

                .copy-button:hover { 
                    background-color: #3498db; /* Blue background on hover */
                    color: white; /* White text on hover */
                }

                .link-input {
                    padding: 8px;
                    border: 1px solid #ddd;
                    border-radius: 4px;
                    width: 70%; /* Adjust width as needed */
                    font-size: 0.9em;
                    color: #555;
                }


                .back-link {
                    display: inline-block;
                    padding: 10px 20px;
                    background-color: #f4f4f4;
                    color: #3498db;
                    text-decoration: none;
                    border-radius: 4px;
                    border: 1px solid #ddd;
                    transition: background-color 0.3s, color 0.3s, border-color 0.3s;
                    margin-top: 20px;
                }

                .back-link:hover {
                    background-color: #ddd;
                    color: #2980b9;
                    border-color: #ccc;
                }


            </style>
            <script>
                function copyToClipboard(id) {
                    var copyText = document.getElementById(id);
                    copyText.select();
                    copyText.setSelectionRange(0, 99999); // For mobile devices
                    document.execCommand("copy");
                    alert("Copied Download Link!");
                }
            </script>
        </head>
        <body>
            <h1>Upload History</h1>
            <table>
                <thead>
                    <tr>
                        <th>Filename</th>
                        <th>File Size (MB)</th>
                        <th>Uploaded At</th>
                        <th>Download Link</th>
                        <th>Copy Link</th>
                    </tr>
                </thead>
                <tbody>)";
        
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
            body += "<td><input type='text' class='link-input' id='" + inputId + "' value='" + fullLink + "' readonly>";
            body += "<button class='copy-button' onclick='copyToClipboard(\"" + inputId + "\")'>Copy Link</button></td>";
            body += "</tr>";
        }
        
        body += R"(
                </tbody>
            </table>
            <a href="/" class="back-link">Back to Home</a>
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