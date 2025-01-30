#include "connection_pool.h"

//Constructor
ConnectionPool::ConnectionPool(size_t poolSize, const std::string& connInfo): poolSize(poolSize), connInfo(connInfo)
{    
    std::cout << "Initializing connection pool with " << poolSize << " connections...\n";
    
    initializePool();

    std::cout << "Connection pool initialized successfully with " << poolSize << " connections.\n";
}

//Since we are using smart pointers
//The custom Deleter function will be called when they go out of scope
//Hence cleaning up the connections for us
ConnectionPool::~ConnectionPool(){
    std::cout << "Closing the connection pool with " << availableConnections.size() << " connections.\n";
}

//Creates the required number of connections and pushes onto the queue
void ConnectionPool::initializePool()
{
    //no need to lock the poolMutex here since main.cpp will only call this constructor once!
    for(size_t i = 0; i < poolSize; ++i){
        std::unique_ptr<PGconn,Deleter> conn(PQconnectdb(connInfo.c_str()),Deleter());
        if(check_pq_status(conn)) availableConnections.push(std::move(conn));
        else{ 
            std::cerr << i << "/" << poolSize << " connections made\n";
            throw std::runtime_error("Error in constructing the pool: " + std::string(PQerrorMessage(conn.get())));
        }
    }
}

//Returns true on successful connection, and false otherwise
bool ConnectionPool::check_pq_status(const std::unique_ptr<PGconn, Deleter>& conn){
    if(PQstatus(conn.get()) != CONNECTION_OK) 
        return false;
    else 
        return true;
}

std::unique_ptr<PGConn, Deleter> get_connection(){
    //locking the mutex as access queue to keep it thread safe
    //using unique_lock so it unlocks as soon as it goes out of scope
    std::unique_lock<std::mutex> lock(poolMutex);
    
    //cv unlocks the mutex and waits if the condition returns false
    poolCV.wait(lock, [this]() { return !availableConnections.empty() });
    
    //move the ptr from queue to this temp unique ptr and return the ptr
    std::unique_ptr<PGConn, Deleter> conn(std::move(availableConnections.front()));
    availableConnections.pop();

    std::cout << "Connection alloted!" << std::endl;
    
    return conn;
}
