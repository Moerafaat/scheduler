//
//  main.cpp
//  scheduler
//
//  Created by Mohammed Anany on 10/12/2021.
//

#include <iostream>
#include "cron_scheduler.hpp"

void testFunction() {
    std::cout << "I'm here boy" << std::endl;
}

int main(int argc, const char * argv[]) {
    CronScheduler scheduler = CronScheduler();
    scheduler.scheduleJob(testFunction, std::chrono::milliseconds(1000), std::chrono::milliseconds(1000000));
    return 0;
}
