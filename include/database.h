#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <libpq-fe.h> // PostgreSQL C API
#include <memory>
#include <iostream>

class Database {
public:
    Database(const std::string& connInfo); // Constructor which connects to the db
    ~Database(); // Destructor (optional, since unique_ptr will handle cleanup)
    bool connect(); // Opens a connection to the PSQL server and checks its status
    bool executeQuery(const std::string& query); // Executes a query (e.g., INSERT, UPDATE, DELETE)
    std::vector<std::vector<std::string>> fetchResult(const std::string& query); // Fetches data using SELECT query

private:
    // Custom deleter for PGconn
    struct Deleter {
        void operator()(PGconn* conn) {
            if (conn) PQfinish(conn);
        }
    };

    std::unique_ptr<PGconn, Deleter> conn; // PSQL connection object with custom deleter
    void checkError(PGresult* res); // Helper function to check if the query was successful
};

#endif // DATABASE_H