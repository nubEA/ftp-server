#ifndef CONNECTION_POOL_H
#define CONNECTION_POOL_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <libpq-fe.h>
#include <chrono>

class ConnectionPool {
public:
    ConnectionPool(size_t poolSize, const std::string& connInfo, int timeout);       
    ~ConnectionPool();                                                  //cleans up and closes all connections
    std::unique_ptr<PGconn, Deleter> get_connection();                   //the thread asks for the connection using this function
    void return_connection(std::unique_ptr<PGconn, Deleter> conn);       //the thread returns the connection using this function

private:
    std::queue<std::unique_ptr<PGconn, Deleter>> availableConnections;  //this is the pool, containing the opened connections
    std::mutex poolMutex;                                               //mutex to ensure thread safe use of the queue
    std::condition_variable poolCV;                                     //cv to notify threads waiting for a connection from the pool whenever available
    size_t poolSize;                                                    //hold pool size                            
    std::string connInfo;                                               //hold connection string
    std::chrono::milliseconds timeoutDuration;                          //if it takes longer than this, we handout connection timed out

    //custom deleter functor for our unique pter, since PGConn doesnt use new and delete, prolly malloc and free
    struct Deleter {
        void operator()(PGconn* conn) {
            if (conn) PQfinish(conn);
        }
    };

    //Checks the status of a PostgreSQL connection.
    // @param conn: A reference to a unique_ptr holding the PGconn object.
    // @return: True if the connection is valid, false otherwise.
    static bool check_pq_status(const std::unique_ptr<PGconn, Deleter>& conn);                                      //helper function to check PQstatus

    //In case an old connection becomes invalid, we replace that connection by creating a new one
    std::unique_ptr<PGconn, Deleter> make_new_connection();

    //initialise the connection pool, add the connection objs to the queue
    void initializePool();                              
};

#endif // CONNECTION_POOL_H