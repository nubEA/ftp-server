#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <libpq-fe.h> // PostgreSQL C API
#include <memory>

class Database{
    public:
        Database(std::string& connInfo);                                            //constructor which connects to the db 
        ~Datbase();                                                                 //cleans up the database by calling PQfinish()
        bool connect();                                                             //opens a connection to the PSQL server
        bool executeQuery(const std::string& query);                                //executes a query
        std::vector<std::string> fetchResult(const std::string& query);             //fetches data using SELECT query
    private:
        std::unique_ptr<PGconn, Deleter> conn;                                                               //PSQL connection object
        void checkError(PGresult* res);                                             //helper function to check if the q was successful
}

struct Deleter{
    void operator()
}

#endif

//research about how to implement distributed file storage system and how it works

struct Deleter {
    //operator() overloading to make this object act as a callable function (aka functor)
    void operator()(PGconn* conn) { 
        if (conn) PQfinish(conn); 
    }
};

Database::Database(const std::string& connInfo): conn(PQconnectdb(connInfo.c_str()), Deleter()) 
{
    //PQ status returns checks if the connection was successful return CONNECTION_OK on success
    if(PQstatus(conn.get()) != CONNECTION_OK){
        //PQerrorMessage() prints out the error messasge in humar readable way
        std::cerr << "Error connecting to DB: " << PQerrorMessage(conn.get()) << std::endl; 
    }
    else std::cout << "Connected to DB!\n";
}



//destructor just for logging purposes, since the smart pointer paired with the custom deleter handles closing the connection!
Database::~Database(){
    std::cout << "Closed the connection!\n";
}


