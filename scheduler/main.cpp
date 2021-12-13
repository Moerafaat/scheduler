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

    scheduler.scheduleJob(testFunctionWithoutParameter, 1, milliseconds(10000), milliseconds(1000000));
    for(int i=2; i<20; i++) {
        scheduler.scheduleJob(testFunctionWithParameter, i, milliseconds(10000), milliseconds(1000000), i);
    }
    
    return 0;
}
