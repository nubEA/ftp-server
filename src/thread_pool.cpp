#include "thread_pool.h"

ThreadPool::ThreadPool(size_t numberOfThreads){
    //initialise the shutdown flag as false
    shutdownFlag = false;
    create_threads(numberOfThreads);
}

ThreadPool::~ThreadPool()
{
    if(!shutdownFlag.load())
    {
        shutdown();
    }
    workerThreads.clear();
}

void ThreadPool::create_threads(size_t numberOfThreads){
    auto workerFn = [this]{worker();};
    for(size_t i = 0; i < numberOfThreads; ++i)
    {
        //using lambda since we need fn pointer in std::thread()
        //since worker is a member fn and we must be associated to an instance of the class
        try
        {
            //std::thread(&ThreadPool::worker,this) is less intuitive
            workerThreads.push_back(std::thread(workerFn));
            // std::string message = std::string("Thread " + std::to_string(i + 1) + " created successfully");
            // std::cout << message << std::endl;
        }
        catch(const std::exception& e)
        {
            std::cerr << "Error in thread creation\n";
            std::cerr << e.what() << '\n';
            throw std::runtime_error("failed to create threads");
        }
    }
}

//Worker fn: waits for the tasks to be in the q, performs the tasks and handles shutdown

void ThreadPool::worker(){
    while(true)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(qMutex);
            //if false returned then we wait
            cv.wait(lock, [this] { return shutdownFlag.load() || !taskQueue.empty();});

            if(shutdownFlag.load() && taskQueue.empty()) break;

            task = std::move(taskQueue.front());
            taskQueue.pop();
        }

        // std::cout << "Executing the task now" << std::endl;

        try
        {
            task();
        }
        catch(const std::exception& e)
        {
            std::cerr << "Func threw an exception\n";
            std::cerr << e.what() << '\n';
        }
        catch(...)
        {
            std::cerr << "Func threw and unknown error\n";
        }
        // std::cout << "Done executing" << std::endl;
    }
}

void ThreadPool::submit(std::function<void()> task) {
    if (shutdownFlag.load()) {
        throw std::runtime_error("Cannot submit tasks after calling shutdown");
    }
    {
        //using std::move instead of q.push to avoid making copies of task which is expensive
        std::unique_lock<std::mutex> lock(qMutex);
        taskQueue.push(std::move(task));
    }
    //unique_lock unlocks by defaul after going out of scope
    cv.notify_one();
}

void ThreadPool::shutdown() {
    {
        std::unique_lock<std::mutex> lock(qMutex);
        shutdownFlag = true;
    }
    cv.notify_all(); // Notify all threads to wake up and check the shutdown flag

    for (auto& thread : workerThreads) {
        //not trying to join threads which have already been detached or threads.
        if (thread.joinable()) {
            try {
                thread.join();
            } catch (const std::exception& e) {
                std::cerr << "Error in joining the thread: " << e.what() << '\n';
            }
        }
    }
}