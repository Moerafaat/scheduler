//
//  job.hpp
//  scheduler
//
//  Created by Mohammed Anany on 13/12/2021.
//

#ifndef job_hpp
#define job_hpp

#include <chrono>
#include <functional>

using namespace std;
using namespace std::chrono;

namespace sched {
class Job {
public:
    Job();
    Job(function<void()> jobFunction, int jobId, milliseconds expectedInterval, milliseconds frequency, bool softDeadline);
    
    bool operator<(const Job& job) const;
    
    function<void()> jobFunction;
    int jobId;
    milliseconds expectedInterval;
    milliseconds frequency;
    bool softDeadline;
};
}

#endif /* job_hpp */
