//
//  thread_pool.cpp
//  scheduler
//
//  Created by Mohammed Anany on 11/12/2021.
//

#include "thread_pool.hpp"

ThreadPool::ThreadPool(unsigned int poolSize) : poolSize(poolSize), workers(new std::thread[poolSize]) {
    for (unsigned int i = 0; i < poolSize; i++) {
        workers[i] = std::thread(&ThreadPool::worker, this);
    }
}

ThreadPool::~ThreadPool() {
    poolRunning = false;
    for (unsigned int i = 0; i < poolSize; i++)
    {
        workers[i].join();
    }
}

template <typename F>
void ThreadPool::dispatchJob(const F &job, std::chrono::milliseconds expectedIntervalMs) {
    if(currentJobs == poolSize) {
        // TODO: Throw -> All resources are busy, try again in {closest estimated completion time}
        throw;
    }
    currentJobs++;
    
    {
        std::unique_lock<std::mutex> lock(jobs_mtx);
        jobs.push(std::function<void()>(job));
    }
    cvJobArrived.notify_one();
}

//template <typename F, typename... A>
//void ThreadPool::dispatchJob(const F &job, const A &...args, std::chrono::milliseconds expectedIntervalMs) {
//    dispatchJob([job, args..., expectedIntervalMs] {
//        job(args..., expectedIntervalMs);
//    });
//}

void ThreadPool::worker() {
    while(poolRunning) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(jobs_mtx);
            cvJobArrived.wait(lock, [this]{return !jobs.empty();});
            job = jobs.front();
            jobs.pop();
        }
        job();
        currentJobs--;
    }
}
