//
//  job.cpp
//  scheduler
//
//  Created by Mohammed Anany on 13/12/2021.
//

#include "job.hpp"

namespace sched {
Job::Job() {
    
}

Job::Job(function<void()> jobFunction, int jobId, milliseconds expectedInterval, milliseconds frequency, bool softDeadline) :
jobFunction(jobFunction), jobId(jobId), expectedInterval(expectedInterval), frequency(frequency), softDeadline(softDeadline) {
    
}

bool Job::operator<(const Job& job) const
{
    return jobId < job.jobId;
}
}
