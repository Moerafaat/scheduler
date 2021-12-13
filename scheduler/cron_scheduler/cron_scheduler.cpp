//
//  cron_scheduler.cpp
//  scheduler
//
//  Created by Mohammed Anany on 11/12/2021.
//

#include "cron_scheduler.hpp"
#include "spdlog/spdlog.h"

namespace sched {
    CronScheduler::CronScheduler(unsigned int poolSize) : threadPool(poolSize) {
        
    }

    CronScheduler::~CronScheduler() {
        spdlog::info("~CronScheduler() - Terminating...");
    }

    void CronScheduler::removeJob(int jobId) {
        
    }

    void CronScheduler::terminate() {
        
    }

    void CronScheduler::validateInputsOrThrow() {
        
    }

    void CronScheduler::dispatcher() {
        
    }
}
