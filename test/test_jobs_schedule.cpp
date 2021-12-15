//
//  test_jobs_schedule.cpp
//  test
//
//  Created by Mohammed Anany on 16/12/2021.
//

#include "catch2/catch.hpp"
#include "cron_scheduler.hpp"
#include <thread>
#include <chrono>

using namespace std;
using namespace std::chrono;
using namespace sched;

namespace test_jobs_schedule {
system_clock::time_point t1;
system_clock::time_point t2;
system_clock::time_point t3;

int invocationCount = 0;
void sleepFor5Seconds() {
    invocationCount++;
    this_thread::sleep_for(seconds(5));
    if(invocationCount == 1) {
        t2 = system_clock::now();
    } else if(invocationCount == 2) {
        t3 = system_clock::now();
    }
}

}

TEST_CASE( "Test that jobs are scheduled correctly", "[CronScheduler]" ) {
    {
        // SETUP
        CronScheduler scheduler;

        // PROCESS
        test_jobs_schedule::t1 = system_clock::now();
        scheduler.scheduleJob(test_jobs_schedule::sleepFor5Seconds, 1, milliseconds(5000), milliseconds(10000), true);

        // TEARDOWN
        this_thread::sleep_for(seconds(17));
        scheduler.clearScheduler();
    }
    
    //ASSERT
    milliseconds diff1 = duration_cast<milliseconds>(test_jobs_schedule::t2 - test_jobs_schedule::t1);
    milliseconds diff2 = duration_cast<milliseconds>(test_jobs_schedule::t3 - test_jobs_schedule::t1);
    REQUIRE(diff1 < milliseconds(5050));  // The 50 milliseconds exist as an acceptable margins
    REQUIRE(diff2 < milliseconds(15050)); // The 50 milliseconds exist as an acceptable margins
}
