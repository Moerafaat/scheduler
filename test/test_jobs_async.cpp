//
//  test_jobs_async.cpp
//  test
//
//  Created by Mohammed Anany on 15/12/2021.
//

#include "catch2/catch.hpp"
#include "cron_scheduler.hpp"
#include <thread>
#include <chrono>

using namespace std;
using namespace std::chrono;
using namespace sched;

system_clock::time_point t1;
system_clock::time_point t2;
system_clock::time_point t3;


void sleepFor2Seconds() {
    this_thread::sleep_for(seconds(2));
    t2 = system_clock::now();
}

void sleepFor5Seconds() {
    this_thread::sleep_for(seconds(5));
    t3 = system_clock::now();
}

TEST_CASE( "Test that jobs run concurrently", "[CronScheduler]" ) {
    {
        // SETUP
        CronScheduler scheduler;

        // PROCESS
        t1 = system_clock::now();
        scheduler.scheduleJob(sleepFor2Seconds, 1, milliseconds(2000), milliseconds(10000), true);
        scheduler.scheduleJob(sleepFor5Seconds, 2, milliseconds(5000), milliseconds(10000), true);


        // TEARDOWN
        this_thread::sleep_for(seconds(1));
        scheduler.clearScheduler();
    }
    
    //ASSERT
    milliseconds diff1 = duration_cast<milliseconds>(t2 - t1);
    milliseconds diff2 = duration_cast<milliseconds>(t3 - t1);
    REQUIRE(diff1 < milliseconds(2050)); // The 50 milliseconds exist as an acceptable margins
    REQUIRE(diff2 < milliseconds(5050)); // The 50 milliseconds exist as an acceptable margins
}
