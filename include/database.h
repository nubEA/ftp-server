#ifndef CONNECTION_POOL_H
#define CONNECTION_POOL_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <libpq-fe.h>
#include <chrono>

/**
 * @class ConnectionPool
 * @brief Manages a pool of database connections to a PostgreSQL database.
 *
 * The ConnectionPool class provides a thread-safe mechanism for managing a pool of
 * database connections. It ensures that connections are reused efficiently and handles
 * connection timeouts and invalid connections gracefully.
 */
class ConnectionPool {
public:
    /**
     * @brief Constructs a ConnectionPool with the specified number of connections and timeout duration.
     * @param poolSize The number of connections in the pool.
     * @param connInfo The connection information string for the PostgreSQL database.
     * @param timeout The timeout duration in milliseconds for waiting for a connection.
     */
    ConnectionPool(size_t poolSize, const std::string& connInfo, int timeout);

    /**
     * @brief Destructs the ConnectionPool and closes all connections.
     *
     * Ensures that all database connections are properly closed when the ConnectionPool
     * object is destroyed.
     */
    ~ConnectionPool();

    /**
     * @brief Gets a connection from the pool.
     * @return A unique pointer to a PGconn object.
     * @throw std::runtime_error if a connection cannot be obtained within the timeout duration.
     *
     * This function is called by threads to obtain a database connection from the pool.
     * If no connections are available within the specified timeout duration, a runtime
     * error is thrown.
     */
    std::unique_ptr<PGconn, Deleter> get_connection();

    /**
     * @brief Returns a connection to the pool.
     * @param conn A unique pointer to a PGconn object.
     *
     * This function is called by threads to return a database connection to the pool
     * after use. The connection is checked for validity before being returned to the pool.
     */
    void return_connection(std::unique_ptr<PGconn, Deleter> conn);

private:
    std::queue<std::unique_ptr<PGconn, Deleter>> availableConnections;  ///< The pool of available database connections.
    std::mutex poolMutex;                                              ///< Mutex to ensure thread-safe access to the pool.
    std::condition_variable poolCV;                                    ///< Condition variable to notify threads waiting for a connection.
    size_t poolSize;                                                   ///< The size of the connection pool.
    std::string connInfo;                                              ///< The connection information string for the PostgreSQL database.
    std::chrono::milliseconds timeoutDuration;                         ///< The timeout duration for waiting for a connection.

    /**
     * @brief Custom deleter functor for PGconn objects.
     *
     * This functor ensures that PGconn objects are properly cleaned up using PQfinish.
     */
    struct Deleter {
        void operator()(PGconn* conn) {
            if (conn) PQfinish(conn);
        }
    };

    /**
     * @brief Checks the status of a PostgreSQL connection.
     * @param conn A reference to a unique_ptr holding the PGconn object.
     * @return True if the connection is valid, false otherwise.
     *
     * This helper function checks the status of a PostgreSQL connection using PQstatus.
     */
    static bool check_pq_status(const std::unique_ptr<PGconn, Deleter>& conn);

    /**
     * @brief Creates a new PostgreSQL connection.
     * @return A unique pointer to a PGconn object.
     *
     * This function creates a new PostgreSQL connection and checks its validity.
     * If the connection is invalid, it returns a nullptr.
     */
    std::unique_ptr<PGconn, Deleter> make_new_connection();

    /**
     * @brief Initializes the connection pool.
     *
     * This function initializes the connection pool by creating the specified number
     * of database connections and adding them to the pool.
     */
    void initializePool();
};

#endif // CONNECTION_POOL_H
