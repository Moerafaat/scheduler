//
//  cron_scheduler.hpp
//  scheduler
//
//  Created by Mohammed Anany on 11/12/2021.
//

#ifndef cron_scheduler_hpp
#define cron_scheduler_hpp

#include "thread_pool.hpp"

class CronScheduler {
public:
    CronScheduler();
    ~CronScheduler();
    
    // Dispatch jobs that have no arguments and no return
    template <typename F>
    void scheduleJob(const F &job, std::chrono::milliseconds expectedIntervalMs, std::chrono::milliseconds frequency);
    
    // Dispatch jobs that have arguments and no return
    template <typename F, typename... A>
    void scheduleJob(const F &job, const A &...args, std::chrono::milliseconds expectedIntervalMs, std::chrono::milliseconds frequency);
    
private:
    ThreadPool threadPool;
};

#endif /* cron_scheduler_hpp */
