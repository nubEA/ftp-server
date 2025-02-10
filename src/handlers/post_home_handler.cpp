// #include "post_home_handler.h"
// #define EIGHT_KB 8192

// void PostHomeHandler::post_home_handler(HttpRequest& req, HttpResponse& res, Database& db){
//     //Get the file struct from the parsed req
//     HttpRequest::UploadedFile file = req.get_all_files().at("file");

//     std::string downloadLink = generate_random_link();
//     store_file_locally(req, file, downloadLink);
//     put_in_db(get_user_id(req.get_token_cookie(),db), file, downloadLink, db);
//     std::cout << "Exiting post home handler\n";
// }

// void PostHomeHandler::store_file_locally(HttpRequest& req, HttpRequest::UploadedFile& file, std::string hashedFileName){
//     //Add path to the file name so it gets saved in the correct path
//     hashedFileName = "/home/harshit/code/ftp-server/uploads/" + hashedFileName; 
//     //First writing to this temp file, to avoid partial writes
//     std::string tempFile = hashedFileName + ".tmp";
//     if(file.type == HttpRequest::FileType::TEXT)
//     {
//         std::string content = req.as_text(file.content);
//         if (!write_text_file(tempFile, hashedFileName, content)) {
//             std::cout << "Text file writing failed.\n";
//             std::remove(tempFile.c_str()); 
//             return;
//         }
//     }
//     else
//     {
//         if(!write_binary_file(tempFile, hashedFileName, file.content)) {
//             std::cout << "Binary file writing failed.\n";
//             std::remove(tempFile.c_str());
//             return;
//         }
//     }
// }

// std::string PostHomeHandler::generate_random_link()
// {
//     unsigned char buff[8];
//     //Generating random byte string of 4 bytes
//     RAND_bytes(buff,8);
//     std::stringstream stream;
//     std::string link{};
//     //Converting byte string to hex to make it a link
//     for(auto& v : buff)
//     {
//         stream << std::hex << std::setw(2) << std::setfill('0') << (static_cast<int>(v));
//     }
//     link = stream.str();
//     return link;
// }

// std::string PostHomeHandler::get_user_id(const std::string& token, Database& db)
// {
//     std::cout << "Fetching User Id\n";
//     std::string stmtName = "get_refresh_token";
//     const char* params[] = {token.c_str()};
//     int paramLen[] = {static_cast<int>(token.length())};
//     int paramFormat[] = {0};
//     int nParams = 1;
//     auto results = db.fetch_results(stmtName,params,paramLen,paramFormat,nParams);
    
//     std::cout << "Returning user id\n";

//     return results[0][0];
// }

// void PostHomeHandler::put_in_db(const std::string& userId, HttpRequest::UploadedFile file, const std::string& link,Database& db) 
// {
//     std::cout << "Trying to put file details in db\n";

//     //Preparing all necessary params for file insertion in db
//     std::string stmtName = "file_insertion";
//     std::string filename = file.filename;
//     size_t filesize = file.size;
//     std::string permission = file.perms;
//     std::string isCompressed = "0";
//     std::string filetype = (file.type == HttpRequest::FileType::TEXT) ? "text" : "binary";
//     std::string fileSizeString = std::to_string(filesize);
    
//     // Include permission in the query parameters
//     const char* params[] = {userId.c_str(), filename.c_str(), fileSizeString.c_str(), link.c_str(), isCompressed.c_str(), permission.c_str(), 
//                             filetype.c_str()};
//     int paramLen[] = {static_cast<int>(userId.length()), static_cast<int>(filename.length()), 
//                       static_cast<int>(fileSizeString.length()), static_cast<int>(link.length()), 
//                       static_cast<int>(isCompressed.length()), static_cast<int>(permission.length()), static_cast<int>(filetype.length())};
//     int paramFormat[] = {0, 0, 0, 0, 0, 0, 0};  // All parameters are text
//     int nParams = 7;

//     if (db.execute_query(stmtName, params, paramLen, paramFormat, nParams)) {
//         std::cout << "File added in db successfully\n";
//     } else {
//         std::cout << "Some error occurred in inserting file in db\n";
//     }
// }
