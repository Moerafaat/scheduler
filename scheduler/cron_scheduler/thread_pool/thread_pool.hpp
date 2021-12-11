//
//  thread_pool.hpp
//  scheduler
//
//  Created by Mohammed Anany on 11/12/2021.
//

#ifndef thread_pool_hpp
#define thread_pool_hpp

#include <thread>
#include <queue>
#include <mutex>
#include <chrono>

class ThreadPool {
public:
    // Defaults to the maximum parallelizable threads on the hardware
    ThreadPool(unsigned int poolSize = std::thread::hardware_concurrency());
    ~ThreadPool();
    
    // Dispatch jobs that have no arguments and no return
    template <typename F>
    void dispatchJob(const F &job, std::chrono::milliseconds expectedIntervalMs);
    
//    // Dispatch jobs that have arguments and no return
//    template <typename F, typename... A>
//    void dispatchJob(const F &job, const A &...args, std::chrono::milliseconds expectedIntervalMs);
    
private:
    void worker();
    
    int poolSize;
    std::unique_ptr<std::thread[]> workers;
    std::atomic<bool> poolRunning = {true};
    std::queue<std::function<void()>> jobs = {};
    std::mutex jobs_mtx = {};
    std::condition_variable cvJobArrived;
    std::atomic<unsigned int> currentJobs = 0;
};

#endif /* thread_pool_hpp */
