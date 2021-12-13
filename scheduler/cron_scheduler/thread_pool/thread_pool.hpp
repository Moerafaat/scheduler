//
//  thread_pool.hpp
//  scheduler
//
//  Created by Mohammed Anany on 11/12/2021.
//

#ifndef thread_pool_hpp
#define thread_pool_hpp

#include "workers_busy_exception.hpp"
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
        
        template <typename F>
        void dispatchJob(const F &job, int jobId, milliseconds expectedIntervalMs, bool softDeadline = true) {
            checkWorkersBusy(jobId, softDeadline);
            
            // Enqueue job for workers to consume
            currentJobs++;
            {
                unique_lock<mutex> lock(jobs_mtx);
                jobs.push(make_pair(make_pair(function<void()>(job), jobId), expectedIntervalMs));
            }
            cvJobArrived.notify_one();
        }
        
    private:
        void worker();                                                      // Method that is deployed in all workers
        void checkWorkersBusy(int jobId, bool softDeadline);                // Checks if workers have capacity to accept new jobs
        void waitForWorkers();                                              // Used during destruction to wait for workers
        void closeWorkers();                                                // Used during destruction to close workers
        void process(pair<pair<function<void()>, int>, milliseconds> job);  // Invoked by workers to process each job
        
        int poolSize;
        unique_ptr<thread[]> workers;
        atomic<bool> poolRunning = {true};
        queue<pair<pair<function<void()>, int>, milliseconds>> jobs = {};
        mutex jobs_mtx = {};
        condition_variable cvJobArrived;
        atomic<unsigned int> currentJobs = 0;
        mutex estimate_mtx = {};
        set<pair<system_clock::time_point, int>> estimatedCompletionTime;
    };
}

#endif /* thread_pool_hpp */
