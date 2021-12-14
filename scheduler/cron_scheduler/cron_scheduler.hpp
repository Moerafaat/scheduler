//
//  cron_scheduler.hpp
//  scheduler
//
//  Created by Mohammed Anany on 11/12/2021.
//

#ifndef cron_scheduler_hpp
#define cron_scheduler_hpp

#include "thread_pool.hpp"
#include <queue>
#include <set>

namespace sched {
class CronScheduler {
public:
    // poolSize -> Defaults to the maximum parallelizable threads on the hardware
    // minAllowedFrequency -> Used to avoid schedule flooding
    CronScheduler(unsigned int poolSize = thread::hardware_concurrency(), milliseconds minAllowedFrequency = milliseconds(5));
    ~CronScheduler();
    
    // Note:
    // To support template functions inside non-template classes, the implementation has to be in the header file
    // The other alternative is to specify all expected template instantiations in the implementation file which isn't feasible here
    
    // Dispatch jobs that have no arguments and no return
    template <typename F>
    void scheduleJob(const F &job, int jobId, milliseconds expectedInterval, milliseconds frequency, bool softDeadline) {
        Job jobObj = Job(job, jobId, expectedInterval, frequency, softDeadline);
        validateInputsOrThrow(jobObj);
        std::chrono::system_clock::time_point next = std::chrono::system_clock::now();
        {
            unique_lock<mutex> lock(scheduleMtx);
            schedule.push(make_pair(next, jobObj));
            scheduledJobs.insert(jobId);
        }
        scheduleCv.notify_one();
    }

    // Dispatch jobs that have arguments and no return
    template <typename F, typename... A>
    void scheduleJob(const F &job, int jobId, milliseconds expectedInterval, milliseconds frequency,  bool softDeadline,
                     const A &...args) {
        scheduleJob([job, args...] {
            job(args...);
        }, jobId, expectedInterval, frequency, softDeadline);
    }
    
    // Remove job from the scheduler
    void removeJob(int jobId);
    
    // Remove all jobs from the scheduler
    void clearScheduler();
    
private:
    void validateInputsOrThrow(Job job);                             // Validate user inputs or throw
    void dispatcher();                                               // Main function that schedules jobs and dispatches them
    void waitForJobs();                                              // Used during destruction to wait for workers
    
    thread dispatcherThread;
    milliseconds minAllowedFrequency;
    ThreadPool threadPool;
    atomic<bool> schedulerRunning = {true};
    mutex scheduleMtx;
    condition_variable scheduleCv;
    condition_variable jobRemovedCv;
    priority_queue<pair<system_clock::time_point, Job>,
                vector<pair<system_clock::time_point, Job>>,
                greater<pair<system_clock::time_point, Job>>> schedule;
    set<int> scheduledJobs;
};
}

#endif /* cron_scheduler_hpp */
