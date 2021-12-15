//
//  cron_scheduler.cpp
//  scheduler
//
//  Created by Mohammed Anany on 11/12/2021.
//

#include "cron_scheduler.hpp"
#include "spdlog/spdlog.h"

namespace sched {
CronScheduler::CronScheduler(unsigned int poolSize, milliseconds minAllowedFrequency) :
threadPool(poolSize), minAllowedFrequency(minAllowedFrequency) {
    spdlog::info("CronScheduler() - Pool size is set to {} and minimum allowed frequency is set to {} ms",
                 poolSize, minAllowedFrequency.count());
    dispatcherThread = thread(&CronScheduler::dispatcher, this);
}

CronScheduler::~CronScheduler() {
    spdlog::info("~CronScheduler() - Terminating...");
    
    spdlog::info("~CronScheduler() - Waiting for jobs to finish...");
    waitForJobs();
    schedulerRunning = false;
    scheduleCv.notify_all();
    spdlog::info("~CronScheduler() - Joining threads...");
    dispatcherThread.join();
    
    spdlog::info("~CronScheduler() - Terminated successfully");
}

void CronScheduler::removeJob(int jobId) {
    {
        unique_lock<mutex> lock(scheduleMtx);
        scheduledJobs.erase(jobId);
    }
    jobRemovedCv.notify_one();
    scheduleCv.notify_one();
}

void CronScheduler::clearScheduler() {
    {
        unique_lock<mutex> lock(scheduleMtx);
        scheduledJobs.clear();
    }
    jobRemovedCv.notify_one();
    scheduleCv.notify_one();
}

void CronScheduler::validateInputsOrThrow(Job job) {
    if(scheduledJobs.count(job.jobId)) {
        throw invalid_argument("Job id " + to_string(job.jobId) + " alread exists. Please try again with a unique job id");
    }
    if(job.frequency < minAllowedFrequency) {
        throw invalid_argument("The minimum allowed frequency is " + to_string(job.frequency.count()) + " ms");
    }
}

void CronScheduler::dispatcher() {
    spdlog::debug("Dispatcher started");
    while(schedulerRunning) {
        pair<system_clock::time_point, Job> job;
        {
            unique_lock<mutex> lock(scheduleMtx);
            if(schedule.empty()) {
                scheduleCv.wait(lock, [this] {return !schedule.empty() || !schedulerRunning;});
            } else {
                scheduleCv.wait_until(lock, schedule.top().first);
            }
            while(!schedule.empty() && !scheduledJobs.count(schedule.top().second.jobId)) {
                schedule.pop();
            }
            if(schedule.empty() || system_clock::now() < schedule.top().first) {
                spdlog::debug("Continue");
                continue;
            }
            job = schedule.top();
            schedule.pop();
        }
        try {
            threadPool.dispatchJob(job.second);
        } catch(WorkersBusyException ex) {
            spdlog::error("A job with a hard deadline couldn't be scheduled on time. Details: {}", ex.what());
        }
        system_clock::time_point next = system_clock::now() + job.second.frequency;
        {
            unique_lock<mutex> lock(scheduleMtx);
            schedule.push(make_pair(next, job.second));
        }
    }
    spdlog::debug("Dispatcher ended");
}

void CronScheduler::waitForJobs() {
    while(true) {
        if (scheduledJobs.empty()) {
            break;
        }
        {
            unique_lock<mutex> lock(scheduleMtx);
            jobRemovedCv.wait(lock);
        }
    }
}
}
