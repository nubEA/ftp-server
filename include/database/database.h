#ifndef DATABASE_H
#define DATABASE_H

#include "connection_pool.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional> // Add this for std::function

/**
 * @class Database
 * @brief Provides an interface for executing queries using the ConnectionPool.
 *
 * This class abstracts the database operations and ensures queries are executed
 * safely using managed connections from the connection pool.
 */
class Database {
public:
    /**
     * @brief Constructs a Database instance with a reference to a ConnectionPool.
     * @param pool A reference to the ConnectionPool instance.
     */
    explicit Database(ConnectionPool& pool);

    /**
     * @brief Executes a query that does not return results (e.g., INSERT, UPDATE, DELETE).
     * @param stmtName The name of the prepared statement.
     * @param params The parameter values.
     * @param paramLen The parameter lengths.
     * @param paramFormat The parameter formats.
     * @param nParams The number of parameters.
     * @return True if the query executed successfully, false otherwise.
     */
    bool execute_query(const std::string& stmtName, const char* params[], int paramLen[], int paramFormat[], int nParams);

    /**
     * @brief Executes a SELECT query and fetches results.
     * @param stmtName The name of the prepared statement.
     * @param params The parameter values.
     * @param paramLen The parameter lengths.
     * @param paramFormat The parameter formats.
     * @param nParams The number of parameters.
     * @return A vector of result rows, each row represented as a vector of strings.
     */
    std::vector<std::vector<std::string>> fetch_results(const std::string& stmtName, const char* params[], int paramLen[], int paramFormat[], int nParams);

private:
    ConnectionPool& connectionPool; ///< Reference to the ConnectionPool instance.
    std::unordered_map<std::string, std::string> preparedStatements; ///< Stores the stmt name, corresponding query as key value pairs.

    /**
     * @brief Helper function to check for query execution errors.
     * @param res Pointer to the PGresult object.
     * @return True if there was an error, false otherwise.
     */
    bool check_query_error(PGresult* res);

    /**
     * @brief Prepares the SQL statements to be used.
     * @param stmtName The statement name which will be executed like select, insert.
     * @param query The SQL query string.
     * @param nParams The number of parameters.
     */
    void prepare_statement(const std::string& stmtName, const std::string& query, int nParams);

    /**
     * @brief Prepares all necessary queries.
     */
    void prepare_queries();

    /**
     * @brief Gets a connection from the pool.
     * @return A unique_ptr to the connection.
     */
    std::unique_ptr<PGconn, ConnectionPool::Deleter> get_connection_from_pool();

    struct QueryInfo{
        std::string stmtName;
        std::string query;
        int nParams;
    };

    // Static member for result deleter
    static const std::function<void(PGresult*)> resultDeleter;
};

#endif // DATABASE_H