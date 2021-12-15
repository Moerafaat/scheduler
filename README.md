# Cron Scheduler
An in-process scheduler developed using C++

## System Overview and Features
### Project Structure
The project structure is as follows:

```
cron_scheduler/
├── cron_scheduler.cpp
├── cron_scheduler.hpp
└── thread_pool
    ├── exceptions
    │   ├── workers_busy_exception.cpp
    │   └── workers_busy_exception.hpp
    ├── models
    │   ├── job.cpp
    │   └── job.hpp
    ├── thread_pool.cpp
    └── thread_pool.hpp
include/
└── spdlog/...
test/
├── include
│   └── catch2
│       └── catch.hpp
├── test_jobs_async.cpp
├── test_jobs_invoked.cpp
└── test_main.cpp
```

### Design
As highlighted in the project structure, the 2 main classes in our implementation are **CronScheduler** and **ThreadPool**.
- **ThreadPool**, as the name suggests, is mainly responsible for governing a pool of threads and pushing tasks to a queue for the workers to pull from. It also attempts at executing all given tasks in a timely manner, otherwise it gives off a warning or throws an exception depending on whether the task has a soft deadline or a hard deadline. The following are the relevant methods of ThreadPool.
    - `ThreadPool(unsigned int poolSize)`
    - `void dispatchJob(Job job)`
- **CronScheduler**, the class exposed to the user, builds on top of the ThreadPool. ThreadPool is not aware of any scheduling that is performed. It only executes tasks that are given to it. The main scheduling effort is done in the CronScheduler. A single thread in the CronScheduler, `dispatcher`, monitors a `priority_queue` which holds all the jobs that are supposed to run along with their next appointed runtime. The `dispatcher` then invokes `dispatchJob` on the ThreadPool, instructing it to run the task when its time comes. The following are the relevant methods of CronScheduler:
    - `CronScheduler(unsigned int poolSize = thread::hardware_concurrency(), milliseconds minAllowedFrequency = milliseconds(5))`
    - `void scheduleJob(const F &job, int jobId, milliseconds expectedInterval, milliseconds frequency, bool softDeadline)`
    - `void scheduleJob(const F &job, int jobId, milliseconds expectedInterval, milliseconds frequency,  bool softDeadline, const A &...args)`
    - `void removeJob(int jobId)`
    - `void clearScheduler()`

- Both ThreadPool and CronScheduler operate using `mutexes` and `condition_variables` in their "waiting" logic to avoid `sleeps` which exhaust CPU resources. They govern shared resources using `unique_locks`, `scoped_locks`, and `atomic` variables, which are the recommended practices in C++ for managing shared resources.

### External Packages
The external packages used in this repository are:
- spdlog for logging: https://github.com/gabime/spdlog
- catch2 for c++ unit testing: https://github.com/catchorg/Catch2

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
## Design Decisions
- To optimize performance, `mutexes` and `condition_variables` are used instead of `sleep` in threads to avoid consuming valuable CPU processing in busy looping. The tradeoff here is that the implementation is more complicated.
- A single thread is used in the CronScheduler pooling over a `priority_queue` to manage scheduling in a scalable and efficient way instead of allocating a thread for each job that arrives.
- A thread pool layer was added to govern the tasks and separated from the scheduling layer to provide more abstraction and easier governance and maintainability.
- The size of the pool is a user-defined parameter that defaults to the number of cores on the machine. This is done to attempt to maximizes the throughput of the jobs that are being processed. 

## Future Improvements
- Currently, jobs with hard deadlines are handled by throwing an exception and are not enqueued as with jobs with soft deadlines. A possible improvement here is to assign priority values to jobs so that higher priority jobs are processed first.
- Currently, jobs with and without arguments are supported. However, support is currently not available except for functions with no return. A future improvement would be to support functions with returns and handle it using `Futures` in C++.
