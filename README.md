# Cron Scheduler
An in-process scheduler developed using C++

## Usage
- First, you need to include the scheduler using `#include "cron_scheduler.hpp"` and create an instance of `CronScheduler`.
- `CronScheduler` resides in its own namespace, which you can use: `using namespace sched;`
- `CronScheduler` has 2 optional parameters in its constructor: poolSize and minAllowedFrequency. These default to `thread::hardware_concurrency()` and `milliseconds(5)` respectively.
- The following code illustrates a simple usage of `CronScheduler`:

```C++
#include "cron_scheduler.hpp"
#include "spdlog/spdlog.h"

using namespace std;
using namespace std::chrono;
using namespace sched;

void testFunctionWithoutParameter() {
    spdlog::info("Inside testFunctionWithoutParameter");
}

void testFunctionWithParameter(int number) {
    spdlog::info("Inside testFunctionWithParameter {}", number);
}

int main(int argc, const char * argv[]) {
    CronScheduler scheduler;

    try {
        // Testing a function that takes no arguments, expected to finish in 10 seconds, runs every 15 seconds, and doesn't have a hard deadline requirement
        scheduler.scheduleJob(testFunctionWithoutParameter, 1, milliseconds(10000), milliseconds(15000), true);
        
        // Testing a function that takes an integer as an argument, expected to finish in 5 seconds, runs every 10 seconds, and does't have hard deadline requirements
        scheduler.scheduleJob(testFunctionWithParameter, 2, milliseconds(5000), milliseconds(10000), true, 2);
    } catch(invalid_argument& ex) {
        spdlog::error("Arguments passed are invalid. Details: {}", ex.what());
    }
    
    // Remove all jobs in the scheduler
    scheduler.clearScheduler();

    spdlog::info("Closing application...");
    return 0;
}
```

## Future Improvements
- Currently, jobs with hard deadlines are handled by throwing an exception and are not enqueued as with jobs with soft deadlines. A possible improvement here is to assign priority values to jobs so that higher priority jobs are processed first.
- Currently, jobs with and without arguments are supported. However, support is currently not available except for functions with no return. A future improvement would be to support functions with returns and handle it using `Futures` in C++.
- To optimize performance, `mutexes` and `condition_variables` instead of using `sleep` in threads to avoid consuming valuable CPU processing in busy looping. The tradeoff here is that the implementation is more complicated and would require care during maintenance.
