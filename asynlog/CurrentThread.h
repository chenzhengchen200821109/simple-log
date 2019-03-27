#ifndef CURRENTTHREAD_H
#define CURRENTTHREAD_H

#include <stdint.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include "Timestamp.h"

namespace muduo
{
namespace CurrentThread
{
    static int tid_ = 0;
    static char tidString_[32] = { '\0', '\0', '\0' };
    static int tidStringLength_ = 6;
    static const char* t_threadName_ = "unknown";

    inline pid_t gettid()
    {
        return static_cast<pid_t>(::syscall(SYS_gettid));
    }

    inline int tid() 
    {
        return (tid_ = gettid());
    }

    inline std::string tidToString() 
    { 
        tidStringLength_ = snprintf(tidString_, sizeof tidString_, "%5d", tid_);
        return tidString_;
    }
    inline const char* tidString() 
    {
        tidToString();
        return tidString_;
    }

    inline int tidStringLength() // for logging
    {
        tidToString();
        return tidStringLength_;
    }

    inline const char* name()
    {
        //return t_threadName;
        return t_threadName_;
    }

    inline bool isMainThread()
    {
        return tid() == ::getpid();
    }

    inline void sleepUsec(int64_t usec) 
    {
        struct timespec ts = { 0, 0 };
        ts.tv_sec = static_cast<time_t>(usec / Timestamp::kMicroSecondsPerSecond);
        ts.tv_nsec = static_cast<long>(usec % Timestamp::kMicroSecondsPerSecond * 1000);
        ::nanosleep(&ts, NULL);
    }
    
} // namespace CurrentThread

} // namespace muduo

#endif
