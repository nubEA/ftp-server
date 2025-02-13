#ifndef GET_DOWNLOAD_HANDLER_H
#define GET_DOWNLOAD_HANDLER_H

#include "http_request.h"      // For HttpRequest class
#include "http_response.h"     // For HttpResponse class
#include "database.h"          // For Database class
#include "util.h"              // For utility functions
#include "auth.h"              // For authentication functions
#include <iostream>            // For std::cout, std::cerr (for logging)
#include <vector>              // For std::vector (to store data)
#include <string>              // For std::string (to handle strings)
#include <cstring>             // For C-string functions
#include <unistd.h>            // For POSIX API functions
#include <sys/types.h>         // For system types
#include <sys/socket.h>        // For socket functions
#include <sys/select.h>        // For select() function
#include <netinet/in.h>        // For internet address structures
#include <netdb.h>             // For network database operations
#include <arpa/inet.h>         // For IP address conversion
#include <atomic>              // For std::atomic (to manage flags)
#include <csignal>             // For signal handling
#include <filesystem>          // For filesystem operations
#include <fstream>             // For file I/O operations

class GetDownloadHandler {
public:
    struct fileInfo {          // Struct to store file information
        std::string userId{};  // User ID associated with the file
        std::string filename{};  // Name of the file
        std::string filesize{};  // Size of the file
        std::string perms{};   // Permissions for the file
    };

    static void get_download_handler(HttpRequest& req, HttpResponse& res, Database& db, int fd);  // Handle download requests

private:
    static struct fileInfo fetch_file_from_link(HttpRequest& req, HttpResponse& res, Database& db, const std::string& link);  // Fetch file info from link
    static void handle_perm_failure(HttpRequest& req, HttpResponse& res, Database& db);  // Handle permission failure
    static void handle_success(HttpRequest& req, HttpResponse& res, Database& db, const std::string& hashedName, fileInfo& info, int fd);  // Handle successful file download
    static void handle_file_not_found(HttpRequest& req, HttpResponse& res, Database& db);  // Handle file not found error
};

#endif // GET_DOWNLOAD_HANDLER_H