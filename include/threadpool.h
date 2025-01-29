#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>          // For std::vector (to store worker threads)
#include <queue>           // For std::queue (to store tasks)
#include <thread>          // For std::thread (to create worker threads)
#include <mutex>           // For std::mutex (to protect shared resources)
#include <condition_variable> // For std::condition_variable (to notify threads)
#include <functional>      // For std::function (to store tasks as callable objects)
#include <atomic>          // For std::atomic (to manage the shutdown flag)
#include <memory>          // For std::unique_ptr or std::shared_ptr (optional, for advanced usage)
#include <utility>         // For std::move (to move tasks into the queue)
#include <iostream>        // For std::cout, std::cerr (for logging and debugging)

class ThreadPool{
    private:
        std::vector<std::thread> workerThreads;         //store worker threads
        std::queue<std::function<void()>> taskQueue;    //queue for the tasks waiting on threads to be free
        std::mutex qMutex;                              //mutex for queue
        std::condition_variable cv;                     //condition variable to notify worker threads
        std::atomic<bool> shutdownFlag;                 //to manage shutdown

        void worker();                                  //the function that each worker thread will execute (e.g., wait for tasks and execute them).


    public:
        //size_t is never negative and its type defs to be big enough by the compiler so it's good for portability
        ThreadPool(size_t numberOfThreads);             //constructor giving the number of working threads
        ~ThreadPool();                                  //destructor to join the threads and free resources
        void submit(std::function<void()> task);        //to add task to the queue
        void shutdown();                                //to shutdown the thread pool
        void create_threads(size_t numberOfThreads);
};

#endif