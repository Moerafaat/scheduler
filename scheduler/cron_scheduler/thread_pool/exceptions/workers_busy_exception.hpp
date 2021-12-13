//
//  workers_busy_exception.hpp
//  scheduler
//
//  Created by Mohammed Anany on 12/12/2021.
//

#ifndef workers_busy_exception_hpp
#define workers_busy_exception_hpp

#include <exception>
#include <chrono>

using namespace std::chrono;

namespace sched {
    class WorkersBusyException : public std::exception
    {
    public:
        WorkersBusyException(milliseconds estimatedDurationInMs);
        const char * what () const throw ();
    private:
        milliseconds estimatedDurationInMs;
    };
}

#endif /* workers_busy_exception_hpp */
