#include "database.h"
#include <iostream>

// Static member definition
const std::function<void(PGresult*)> Database::resultDeleter = [](PGresult* res) { PQclear(res); };

// Constructor
Database::Database(ConnectionPool& pool) : connectionPool(pool) {
    prepare_queries();
}

std::unique_ptr<PGconn, ConnectionPool::Deleter> Database::get_connection_from_pool() {
    std::unique_ptr<PGconn, ConnectionPool::Deleter> conn(connectionPool.get_connection());

    // Ensure connection is valid
    if (!connectionPool.check_pq_status(conn)) {
        connectionPool.return_connection(std::move(conn));
        throw std::runtime_error("Error getting a connection from the pool: " + std::string(PQerrorMessage(conn.get())));
    }
    return conn;
}

void Database::prepare_queries() {
    // Hold all the queries to prepare
    static const std::vector<Database::QueryInfo> queryList = {
        {"user_reg", "INSERT INTO users (username, password_hash) VALUES ($1, $2);", 2},
        {"user_login", "SELECT id, password_hash FROM users WHERE username = $1;", 1},
        {"user_name_available", "SELECT COUNT(*) FROM users WHERE username = $1;", 1},
        
        {"insert_refresh_token", "INSERT INTO user_sessions (user_id, refresh_token_hash, expires_at) VALUES ($1, $2, $3);", 3},
        {"get_refresh_token", "SELECT id FROM user_sessions WHERE user_id = $1 AND refresh_token_hash = $2;", 2},
        {"delete_refresh_token", "DELETE FROM user_sessions WHERE user_id = $1 AND refresh_token_hash = $2;", 2},
        {"delete_all_tokens_for_user", "DELETE FROM user_sessions WHERE user_id = $1;", 1},

        {"file_insertion", "INSERT INTO files (user_id, filename, file_size, download_link, is_compressed) VALUES ($1, $2, $3, $4, $5);", 5},
        {"file_retrieval", "SELECT id, filename, file_size, upload_timestamp, download_link, is_compressed FROM files WHERE download_link = $1;", 1},
        {"file_retrieval_by_id", "SELECT id, filename, file_size, upload_timestamp, download_link, is_compressed FROM files WHERE user_id = $1;", 1},
        {"file_compression", "UPDATE files SET is_compressed = $1 WHERE id = $2;", 2},
        {"file_deletion", "DELETE FROM files WHERE id = $1;", 1},
        {"user_info", "SELECT id, username, password_hash FROM users WHERE username = $1;", 1},
        {"count_user_file", "SELECT COUNT(*) FROM files WHERE user_id = $1;", 1},
        {"all_files", "SELECT id, filename, file_size, upload_timestamp, download_link, is_compressed FROM files WHERE user_id = $1 ORDER BY upload_timestamp DESC;", 1}
    };


    // Preparing each query
    //Preparing statements are connection specific, so we need to prepare for each connection in the pool
    for (size_t i = 0; i < connectionPool.get_pool_size(); ++i) {
        std::unique_ptr<PGconn, ConnectionPool::Deleter> conn(get_connection_from_pool());
        for (const auto& v : queryList) {
            std::unique_ptr<PGresult, decltype(resultDeleter)> res(
                PQprepare(conn.get(), v.stmtName.c_str(), v.query.c_str(), v.nParams, nullptr),
                resultDeleter
            );

            if (PQresultStatus(res.get()) != PGRES_COMMAND_OK) {
                connectionPool.return_connection(std::move(conn));
                throw std::runtime_error("Error in preparing the query: " + std::string(PQresultErrorMessage(res.get())));
            }
        }
        connectionPool.return_connection(std::move(conn));
    }
}

void Database::prepare_statement(const std::string& stmtName, const std::string& query, int nParams) {
    // Request a connection from the pool
    std::unique_ptr<PGconn, ConnectionPool::Deleter> conn(get_connection_from_pool());

    // Prepare the statements
    //decltype basically deduces the correct type of resultDeleter
    std::unique_ptr<PGresult, decltype(resultDeleter)> res(PQprepare(conn.get(), stmtName.c_str(), query.c_str(), nParams, nullptr), resultDeleter);

    // Checking if the statement got prepared
    if (PQresultStatus(res.get()) != PGRES_COMMAND_OK) {
        connectionPool.return_connection(std::move(conn));
        throw std::runtime_error("Error in preparing the query: " + std::string(PQresultErrorMessage(res.get())));
    }

    preparedStatements[stmtName] = query;

    // Return the connection
    connectionPool.return_connection(std::move(conn));
}

// Execute queries
bool Database::execute_query(const std::string& stmtName, const char* params[], int paramLen[], int paramFormat[], int nParams) {
    // Obtain a connection from the pool
    std::unique_ptr<PGconn, ConnectionPool::Deleter> conn(get_connection_from_pool());

    // Executing the query using PQexecPrepared
    // Storing the query in PGresult smart ptr with custom deleter
    std::unique_ptr<PGresult, decltype(resultDeleter)> res(PQexecPrepared(conn.get(), stmtName.c_str(), nParams, params, paramLen, paramFormat, 0), resultDeleter);
    std::cout << "Executing query: " << stmtName << "\nParameters: " << params << std::endl;

    // Checking if the query executed successfully
    if (PQresultStatus(res.get()) != PGRES_COMMAND_OK) {
        connectionPool.return_connection(std::move(conn));
        throw std::runtime_error("Error in executing the db command: " + std::string(PQresultErrorMessage(res.get())));
    }

    std::cout << "Query executed successfully\n";
    connectionPool.return_connection(std::move(conn));
    std::cout << "Connection returned\n";
    return true;
}

std::vector<std::vector<std::string>> Database::fetch_results(const std::string& stmtName, const char* params[], int paramLen[], int paramFormat[], int nParams) {
    // Obtain a connection from the pool
    std::unique_ptr<PGconn, ConnectionPool::Deleter> conn(get_connection_from_pool());

    // Executing the query using PQexecPrepared
    // Storing the query in PGresult smart ptr with custom deleter
    std::unique_ptr<PGresult, decltype(resultDeleter)> res(PQexecPrepared(conn.get(), stmtName.c_str(), nParams, params, paramLen, paramFormat, 0), resultDeleter);
    std::cout << "Executing query: " << stmtName << "\nParameters: " << params << std::endl;

    // Checking if the query executed successfully
    if (PQresultStatus(res.get()) != PGRES_TUPLES_OK) {
        connectionPool.return_connection(std::move(conn));
        throw std::runtime_error("Error in executing the db command: " + std::string(PQresultErrorMessage(res.get())));
    }

    // Get numRows and numCol for the SELECT query
    int numCol = PQnfields(res.get());
    int numRow = PQntuples(res.get());
    std::vector<std::vector<std::string>> resultVector;

    for (int i = 0; i < numRow; ++i) {
        std::vector<std::string> rowVector;
        for (int j = 0; j < numCol; ++j) {
            // Get the value at (row, col)
            rowVector.push_back(PQgetisnull(res.get(), i, j) ? "NULL" : PQgetvalue(res.get(), i, j));
        }
        resultVector.push_back(rowVector);
    }

    std::cout << "Query executed successfully\n";
    connectionPool.return_connection(std::move(conn));
    std::cout << "Connection returned\n";
    return resultVector;
}