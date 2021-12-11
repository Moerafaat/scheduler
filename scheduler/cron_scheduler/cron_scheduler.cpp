//
//  cron_scheduler.cpp
//  scheduler
//
//  Created by Mohammed Anany on 11/12/2021.
//

#include "cron_scheduler.hpp"
#include <iostream>
using namespace std;

CronScheduler::CronScheduler() {
    std::cout << "Hello Constructor" << std::endl;
}

CronScheduler::~CronScheduler() {
    
}

template <typename F>
void CronScheduler::scheduleJob(const F &job, std::chrono::milliseconds expectedIntervalMs, std::chrono::milliseconds frequency) {
    threadPool.dispatchJob(job, expectedIntervalMs);
}

template <typename F, typename... A>
void CronScheduler::scheduleJob(const F &job, const A &...args,
                                std::chrono::milliseconds expectedIntervalMs, std::chrono::milliseconds frequency) {
    scheduleJob([job, args..., expectedIntervalMs, frequency] {
        job(args..., expectedIntervalMs, frequency);
    });
}
