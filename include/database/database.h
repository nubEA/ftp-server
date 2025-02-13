#ifndef DATABASE_H
#define DATABASE_H

#include "connection_pool.h"  // For ConnectionPool class
#include <string>             // For std::string (to handle strings)
#include <vector>             // For std::vector (to store result rows)
#include <unordered_map>      // For std::unordered_map (to store prepared statements)
#include <functional>         // For std::function (to store result deleter)

class Database {
public:
    explicit Database(ConnectionPool& pool);  // Constructor with ConnectionPool reference

    bool execute_query(const std::string& stmtName, const char* params[], int paramLen[], int paramFormat[], int nParams);  // Execute a query without results
    std::vector<std::vector<std::string>> fetch_results(const std::string& stmtName, const char* params[], int paramLen[], int paramFormat[], int nParams);  // Execute a SELECT query and fetch results

private:
    ConnectionPool& connectionPool;  // Reference to the ConnectionPool instance
    std::unordered_map<std::string, std::string> preparedStatements;  // Map of prepared statements

    bool check_query_error(PGresult* res);  // Check for query execution errors
    void prepare_statement(const std::string& stmtName, const std::string& query, int nParams);  // Prepare a SQL statement
    void prepare_queries();  // Prepare all necessary queries
    std::unique_ptr<PGconn, ConnectionPool::Deleter> get_connection_from_pool();  // Get a connection from the pool

    struct QueryInfo {        // Struct to store query information
        std::string stmtName;
        std::string query;
        int nParams;
    };

    static const std::function<void(PGresult*)> resultDeleter;  // Static member for result deleter
};
#endif // DATABASE_H