#ifndef SERVER_H
#define SERVER_H

#include <iostream>            // For std::cout, std::cerr (for logging and debugging)
#include <vector>              // For std::vector (to store data)
#include <string>             // For std::string (to handle strings)
#include <cstring>             // For C-string functions
#include <unistd.h>            // For POSIX API functions
#include <sys/types.h>         // For system types
#include <sys/socket.h>        // For socket functions
#include <sys/select.h>        // For select() function
#include <netinet/in.h>        // For internet address structures
#include <netdb.h>             // For network database operations
#include <arpa/inet.h>         // For IP address conversion
#include <cstdlib>             // For standard library functions
#include <atomic>              // For std::atomic (to manage shutdown flag)
#include <csignal>             // For signal handling
#include "thread_pool.h"       // For ThreadPool class
#include "connection_pool.h"   // For ConnectionPool class
#include "http_request.h"      // For HTTP request handling
#include "http_response.h"     // For HTTP response handling
#include "router.h"            // For routing requests
#include "http_parser.h"       // For HTTP parsing
#include "database.h"          // For database operations
#include "multipart_parser.h"  // For multipart form data parsing

class Server {
public:
    int pipefd[2];             // Pipe file descriptors for signaling
    Server(char* port);        // Constructor to initialize server and start listening
    ~Server();                 // Destructor to clean up resources
    void stop();               // Shutdown the server in case of critical errors
    Database& get_database();  // Getter for the database object

private:
    ThreadPool pool;           // Thread pool to handle client requests
    std::atomic<bool> shutServer;  // Atomic flag to signal server shutdown
    std::string connInfo;      // Connection string for the database
    ConnectionPool connPool;   // Connection pool for database connections
    Database db;               // Database object for query handling

    int create_bind_socket(char* port);  // Create and bind a socket to the specified port
    void start_listening(int sockfd, char* port);  // Start listening for incoming connections
    int accept_connection(int sockfd);  // Accept a new client connection
    void handle_request(int sockfd);    // Handle incoming client requests
    void set_up_signal_handler();       // Set up signal handler for Ctrl+C
};
#endif // SERVER_H