//
//  thread_pool.hpp
//  scheduler
//
//  Created by Mohammed Anany on 11/12/2021.
//

#ifndef thread_pool_hpp
#define thread_pool_hpp

#include "workers_busy_exception.hpp"
#include "job.hpp"
#include "spdlog/spdlog.h"
#include <thread>
#include <queue>
#include <mutex>
#include <chrono>
#include <set>

using namespace std;
using namespace std::chrono;

namespace sched {
class ThreadPool {
public:
    ThreadPool(unsigned int poolSize);
    ~ThreadPool();
    
    void dispatchJob(Job job) {
        checkWorkersBusy(job);
        
        // Enqueue job for workers to consume
        currentJobs++;
        {
            unique_lock<mutex> lock(jobsMtx);
            jobs.push(job);
        }
        jobArrivedCv.notify_one();
    }
    
private:
    void worker();                                                      // Method that is deployed in all workers
    void checkWorkersBusy(Job job);                                     // Checks if workers have capacity to accept new jobs
    void waitForWorkers();                                              // Used during destruction to wait for workers
    void closeWorkers();                                                // Used during destruction to close workers
    void process(Job job);                                              // Invoked by workers to process each job
    
    int poolSize;
    unique_ptr<thread[]> workers;
    atomic<bool> poolRunning = {true};
    queue<Job> jobs = {};
    mutex jobsMtx = {};
    condition_variable jobArrivedCv;
    condition_variable jobRemovedCv;
    atomic<unsigned int> currentJobs = 0;
    mutex estimateMtx = {};
    set<pair<system_clock::time_point, int>> estimatedCompletionTime;
};
}

#endif /* thread_pool_hpp */
