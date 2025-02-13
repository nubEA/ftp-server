#ifndef CONNECTION_POOL_H
#define CONNECTION_POOL_H

#include <queue>               // For std::queue (to store available connections)
#include <mutex>               // For std::mutex (to protect shared resources)
#include <condition_variable>  // For std::condition_variable (to notify threads waiting for connections)
#include <memory>              // For std::unique_ptr (to manage connection objects)
#include <libpq-fe.h>          // For PostgreSQL connection functions
#include <chrono>              // For std::chrono (to handle timeouts)
#include <iostream>            // For std::cout, std::cerr (for logging and debugging)

class ConnectionPool {
public:
    struct Deleter {           // Custom deleter for PGconn objects
        void operator()(PGconn* conn) {
            if (conn) PQfinish(conn);  // Properly clean up PGconn objects
        }
    };

    ConnectionPool(size_t poolSize, const std::string& connInfo, int timeout);  // Constructor with pool size, connection info, and timeout
    ~ConnectionPool();        // Destructor to close all connections

    std::unique_ptr<PGconn, Deleter> get_connection();  // Get a connection from the pool
    void return_connection(std::unique_ptr<PGconn, Deleter> conn);  // Return a connection to the pool
    static bool check_pq_status(const std::unique_ptr<PGconn, Deleter>& conn);  // Check if a connection is valid
    size_t get_pool_size();    // Get the size of the connection pool

private:
    std::queue<std::unique_ptr<PGconn, Deleter>> availableConnections;  // Queue of available connections
    std::mutex poolMutex;      // Mutex for thread-safe access to the pool
    std::condition_variable poolCV;  // Condition variable for connection availability
    size_t poolSize;           // Size of the connection pool
    std::string connInfo;      // Connection info string for PostgreSQL
    std::chrono::milliseconds timeoutDuration;  // Timeout duration for waiting

    std::unique_ptr<PGconn, Deleter> make_new_connection();  // Create a new PostgreSQL connection
    void initializePool();     // Initialize the connection pool with connections
};
#endif // CONNECTION_POOL_H