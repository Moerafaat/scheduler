//
//  thread_pool.cpp
//  scheduler
//
//  Created by Mohammed Anany on 11/12/2021.
//

#include "thread_pool.hpp"

namespace sched {
    ThreadPool::ThreadPool(unsigned int poolSize) : poolSize(poolSize), workers(new thread[poolSize]) {
        spdlog::info("ThreadPool() - Pool size is set to {}", poolSize);
        
        // Create worker threads
        for (unsigned int i = 0; i < poolSize; i++) {
            workers[i] = thread(&ThreadPool::worker, this);
        }
    }

    ThreadPool::~ThreadPool() {
        spdlog::info("~ThreadPool() - Terminating...");
        
        waitForWorkers(); // Wait for all current workers to finish
        closeWorkers();   // Close all workers
        spdlog::info("All workers terminated.");
    }

    void ThreadPool::worker() {
        while(poolRunning) {
            pair<pair<function<void()>, int>, milliseconds> job;
            {
                unique_lock<mutex> lock(jobs_mtx);
                cvJobArrived.wait(lock, [this]{return (!jobs.empty() || !poolRunning);});
                if(!jobs.empty() && poolRunning) {
                    job = jobs.front();
                    jobs.pop();
                }
            }
            if(job.first.first != nullptr) {
                process(job);
            }
        }
    }

    void ThreadPool::checkWorkersBusy(int jobId, bool softDeadline) {
        // All workers are busy
        if(currentJobs >= poolSize) {
            pair<system_clock::time_point, int> estimate;
            estimate = *estimatedCompletionTime.begin();
            
            if(softDeadline) {
                 spdlog::warn("Job {} will be delayed due to busy workers. Next available slot will be in {} ms",
                              jobId, duration_cast<milliseconds>(estimate.first - system_clock::now()).count());
            } else {
                 spdlog::error("All workers are busy. Job {} was set to have a hard deadline.", jobId);
                throw WorkersBusyException(duration_cast<milliseconds>(system_clock::now() - estimate.first));
            }
        }
    }

    void ThreadPool::waitForWorkers() {
        while(true) {
            if (currentJobs == 0) {
                break;
            }
            this_thread::sleep_for(microseconds(1000));
        }
    }

    void ThreadPool::closeWorkers() {
        poolRunning = false;
        cvJobArrived.notify_all();
        for (unsigned int i = 0; i < poolSize; i++)
        {
            workers[i].join();
        }
    }

    void ThreadPool::process(pair<pair<function<void()>, int>, milliseconds> job) {
        // Update estimated completion time
        pair<system_clock::time_point, int> expectedEndTime = make_pair(system_clock::now() + job.second, job.first.second);
        {
            scoped_lock<mutex> lock(estimate_mtx);
            estimatedCompletionTime.insert(expectedEndTime);
        }
        
        // Run job and track run time
        system_clock::time_point startTime = system_clock::now();
        job.first.first();
        milliseconds runTime = duration_cast<milliseconds>(system_clock::now() - startTime);
        
        // Remove from estimated completion time
        {
            scoped_lock<mutex> lock(estimate_mtx);
            estimatedCompletionTime.erase(estimatedCompletionTime.find(expectedEndTime));
        }
        
        spdlog::info("Job {} was estimated to finish in {} ms. Actual run time: {} ms.", job.first.second, job.second.count(), runTime.count());
        
        currentJobs--;
    }
}
