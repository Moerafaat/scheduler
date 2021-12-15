//
//  main.cpp
//  scheduler
//
//  Created by Mohammed Anany on 10/12/2021.
//

#include "cron_scheduler.hpp"
#include "spdlog/spdlog.h"

using namespace std;
using namespace std::chrono;
using namespace sched;

void testFunctionWithoutParameter_invoked() {
    spdlog::info("Inside testFunctionWithoutParameter");
    for(int i=0; i<10; i++) {
        this_thread::sleep_for(milliseconds(1000));
    }
}

void testFunctionWithParameter_invoked(int number) {
    spdlog::info("Inside testFunctionWithParameter {}", number);
    for(int i=0; i<5; i++) {
        this_thread::sleep_for(milliseconds(1000));
    }
}

int main(int argc, const char * argv[]) {
    CronScheduler scheduler;

    // Testing a function that takes no arguments, expected to finish in 10 seconds, runs every 15 seconds, and doesn't have a hard deadline requirement
    try {
        scheduler.scheduleJob(testFunctionWithoutParameter, 1, milliseconds(10000), milliseconds(15000), true);
    } catch(invalid_argument& ex) {
        spdlog::error("Arguments passed are invalid. Details: {}", ex.what());
    }

    // Testing 3 functions that take an integer as an argument, expected to finish in 5 seconds, run every 10 seconds, and don't have hard deadline requirements
    for(int i=1; i<5; i++) {
        try {
            scheduler.scheduleJob(testFunctionWithParameter, i, milliseconds(5000), milliseconds(10000), true, i);
        } catch(invalid_argument& ex) {
            spdlog::error("Arguments passed are invalid. Details: {}", ex.what());
        }
    }

    this_thread::sleep_for(seconds(5));

    // Remove job with id = 1 from the scheduler
    spdlog::info("Removing job with ID = 1 from the scheduler");
    scheduler.removeJob(1);

    this_thread::sleep_for(seconds(30));

    // Remove all jobs in the scheduler
    spdlog::info("Removing all jobs from the scheduler");
    scheduler.clearScheduler();

    spdlog::info("Closing application...");
    return 0;
}
