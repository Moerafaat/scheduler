//
//  workers_busy_exception.cpp
//  scheduler
//
//  Created by Mohammed Anany on 12/12/2021.
//

#include "workers_busy_exception.hpp"
#include <string>

using namespace std;

namespace sched {
WorkersBusyException::WorkersBusyException(milliseconds estimatedDurationInMs) : estimatedDurationInMs(estimatedDurationInMs) {
    
}

const char * WorkersBusyException::what () const throw ()
{
    string msg = "All workers are currently busy. Next available slot will be in " + to_string(estimatedDurationInMs.count());
    return strdup(msg.c_str());
}
}
