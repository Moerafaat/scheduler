//
//  test_jobs_invoked.cpp
//  test
//
//  Created by Mohammed Anany on 14/12/2021.
//

#include "catch2/catch.hpp"
#include "cron_scheduler.hpp"

using namespace sched;

namespace test_jobs_invoked {
bool invoked = false;
int value = 0;

void testFunctionWithoutParameter() {
    invoked = true;
}

void testFunctionWithParameter(int val) {
    invoked = true;
    value = val;
}
}

TEST_CASE( "Test that functions are invoked", "[CronScheduler]" ) {
    SECTION("Test that function without parameter is invoked") {
        // SETUP
        test_jobs_invoked::invoked = false;
        CronScheduler scheduler;

        // PROCESS
        scheduler.scheduleJob(test_jobs_invoked::testFunctionWithoutParameter, 1, milliseconds(1), milliseconds(5000), true);

        // TEARDOWN
        this_thread::sleep_for(seconds(1));
        scheduler.clearScheduler();
        
        //ASSERT
        REQUIRE(test_jobs_invoked::invoked);
    }
    
    SECTION("Test that function with parameter is invoked") {
        // SETUP
        test_jobs_invoked::invoked = false;
        CronScheduler scheduler;

        // PROCESS
        scheduler.scheduleJob(test_jobs_invoked::testFunctionWithParameter, 1, milliseconds(1), milliseconds(5000), true, 2);

        // TEARDOWN
        this_thread::sleep_for(seconds(1));
        scheduler.clearScheduler();
        
        //ASSERT
        REQUIRE(test_jobs_invoked::invoked);
        REQUIRE(test_jobs_invoked::value == 2);
    }
}
