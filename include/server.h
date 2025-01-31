#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <atomic>
#include <csignal>
#include "thread_pool.h"

class Server {
public:
    int pipefd[2];
    Server(char* port);                 // Constructor: Initializes the server and starts listening on the specified port.
    ~Server();
    void stop();                        // used ot shutdown the server in case of critical errors
private:
    ThreadPool pool;                    // Thread pool to handle client requests concurrently.
    std::atomic<bool> shutServer;

    int create_bind_socket(char* port); // Creates and binds a socket to the specified port.
    void start_listening(int sockfd, char* port);   // Starts listening for incoming connections on the given socket.
    int accept_connection(int sockfd);  // Accepts a new client connection and returns the client socket file descriptor.
    void handle_request(int sockfd);    // Handles incoming client requests by submitting tasks to the thread pool.
    void set_up_signal_handler();
};

#endif // SERVER_H