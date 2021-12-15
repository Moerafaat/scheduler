//
//  unit_test_1.cpp
//  test
//
//  Created by Mohammed Anany on 14/12/2021.
//

#include "catch2/catch.hpp"
#include "cron_scheduler.hpp"

using namespace sched;

bool invoked = false;
void testFunctionWithoutParameter() {
    invoked = true;
}

TEST_CASE( "Test that function without parameter is invoked", "[CronScheduler]" ) {
    // SETUP
    CronScheduler scheduler;
    
    // PROCESS
    scheduler.scheduleJob(testFunctionWithoutParameter, 1, milliseconds(1), milliseconds(5000), true);
    
    // TEARDOWN
    scheduler.clearScheduler();
    
    //ASSERT
    REQUIRE(invoked);
    
}
