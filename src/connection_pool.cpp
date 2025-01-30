#include "connection_pool.h"

//work on handling connection timeout and creating new connection if an existing connection somehow is no longer valid

//Constructor
ConnectionPool::ConnectionPool(size_t poolSize, const std::string& connInfo, int timeout): poolSize(poolSize), connInfo(connInfo), timeoutDuration(timeout)
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

std::unique_ptr<PGConn, Deleter> ConnectionPool::make_new_connection(){
    std::unique_ptr<PGConn, Deleter> conn(PQconnectdb(connInfo.c_str()),Deleter());
    //checking validity of connection
    if(check_pq_status(conn)){
        std::cout << "A replacement connection created successfully\n";
        return conn;
    }
    else{ 
        std::cerr << "Error in finding a replacement" << std::endl;
        return nullptr;
    }
}

std::unique_ptr<PGConn, Deleter> ConnectionPool::get_connection(){
    //locking the mutex as access queue to keep it thread safe
    //using unique_lock so it unlocks as soon as it goes out of scope
    std::unique_lock<std::mutex> lock(poolMutex);

    std::cout << "Waiting for a connection...." << std::endl;
    try{
         //cv unlocks the mutex and waits if the condition returns false
        if(poolCV.wait_for(lock, timeoutDuration, [this]() { return !availableConnections.empty() }) == std::cv_status::timeout){
            std::cout << "Connection timed out\n";
            throw std::runtime_error("Connection timed out\n");
        }
        
        //move the ptr from queue to this temp unique ptr and return the ptr
        std::unique_ptr<PGConn, Deleter> conn(std::move(availableConnections.front()));
        availableConnections.pop();

        //If the connection has somehow become invalid, create a new one to replace it in the pool
        if(!check_pq_status(conn)){
            std::cout << "Invalid connection found, replacing it...\n";
            conn = make_new_connection();

            //we got a nullptr from the replacement meaning it failed
            if(!conn){
                throw std::runtime_error("Error in finding a replacement");
            }
        }    

        std::cout << "Connection alloted!" << std::endl;
        return conn;

    }
    catch(const std::exception& e)
    {
        std::cout << "Error in getting a connection: " << std::string(e.what());
        throw;
    }

}

//The thread moves the conn ptr to this fn which then adds the conn back in the pool
void ConnectionPool::return_connection(std::unique_ptr<PGConn, Deleter> conn){ 
    std::cout << "Received the connection back\n";
    
    //locking the mutex as we are about to push into the queue
    std::unique_lock<std::mutex> lock(poolMutex);

    if(!check_pq_status(conn)){
        std::cout << "The connection is now invalid, creating a replacement\n";
        conn = make_new_connection();
        //if the replacement fails
        if(!conn)
        {
            std::cerr << "Replacement failed, working with 1 lesser connection in the pool now\n";
            return;
        }
    }

    //moving the conn to the pool
    availableConnections.push(std::move(conn));
    std::cout << "Connection returned to the pool successfuly\n";
    //notify the ones waiting on connections to check the pool
    poolCV.notify_one();
}
