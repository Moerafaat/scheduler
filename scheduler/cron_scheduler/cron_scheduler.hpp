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

namespace sched {
    class CronScheduler {
    public:
        // Defaults to the maximum parallelizable threads on the hardware
        CronScheduler(unsigned int poolSize = std::thread::hardware_concurrency());
        ~CronScheduler();
        
        // Note:
        // To support template functions inside non-template classes, the implementation has to be in the header file
        // The other alternative is to specify all expected template instantiations in the implementation file which isn't feasible here
        
        // Dispatch jobs that have no arguments and no return
        template <typename F>
        void scheduleJob(const F &job, int jobId, std::chrono::milliseconds expectedIntervalMs, std::chrono::milliseconds frequency) {
    //        validateInputsOrThrow();
            threadPool.dispatchJob(job, jobId, expectedIntervalMs);
    //        std::chrono::system_clock::time_point end = std::chrono::system_clock::now() + std::chrono::milliseconds(15000);
    //        schedule.push(make_pair(end, jobId));
        }

        // Dispatch jobs that have arguments and no return
        template <typename F, typename... A>
        void scheduleJob(const F &job, int jobId, std::chrono::milliseconds expectedIntervalMs, std::chrono::milliseconds frequency,
                         const A &...args) {
            scheduleJob([job, args...] {
                job(args...);
            }, jobId, expectedIntervalMs, frequency);
        }
        
        void removeJob(int jobId);
        
        void terminate();
        
    private:
        void validateInputsOrThrow();
        void dispatcher();
        
        ThreadPool threadPool;
        std::priority_queue<std::pair<std::chrono::system_clock::time_point, int>> schedule;
    };
}

#endif /* cron_scheduler_hpp */
