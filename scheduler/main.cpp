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

void testFunctionWithoutParameter() {
    spdlog::info("Inside testFunctionWithoutParameter");
    for(int i=0; i<10; i++) {
        this_thread::sleep_for(milliseconds(1000));
    }
}

void testFunctionWithParameter(int number) {
    spdlog::info("Inside testFunctionWithParameter {}", number);
    for(int i=0; i<10; i++) {
        this_thread::sleep_for(milliseconds(1000));
    }
}

int main(int argc, const char * argv[]) {
    CronScheduler scheduler = CronScheduler();

    scheduler.scheduleJob(testFunctionWithoutParameter, 1, milliseconds(10000), milliseconds(15000), true);
    for(int i=2; i<5; i++) {
        scheduler.scheduleJob(testFunctionWithParameter, i, milliseconds(10000), milliseconds(15000), true, i);
    }
   
    this_thread::sleep_for(seconds(5));
    scheduler.removeJob(1);
    this_thread::sleep_for(seconds(30));
    scheduler.clearScheduler();
    spdlog::info("Closing application...");
    return 0;
}
