#include "Thread.h"
#include "Exception.h"
//#include "Logging.h"
#include "CurrentThread.h"
#include <type_traits>
#include <memory>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdlib.h>
#include <linux/unistd.h>
#include "Timestamp.h"

namespace muduo
{
    const bool sameType = std::is_same<int, pid_t>::value;
    static_assert(sameType, "int and pid_t are not the same type");

namespace detail
{

    // get thread identification
    pid_t gettid()
    {
        return static_cast<pid_t>(::syscall(SYS_gettid));
    }

    // sleep a while 
    void sleepUsec(int64_t usec)
    {
        struct timespec ts = { 0, 0 };
        ts.tv_sec = static_cast<time_t>(usec / Timestamp::kMicroSecondsPerSecond);
        ts.tv_nsec = static_cast<long>(usec % Timestamp::kMicroSecondsPerSecond * 1000);
        ::nanosleep(&ts, NULL);
    }
    
    struct ThreadData
    {
        typedef muduo::Thread::ThreadFunc ThreadFunc;
        ThreadFunc func_;
        string name_;
        pid_t* tid_;
        CountDownLatch* latch_;

        ThreadData(const ThreadFunc& func, const string& name, pid_t* tid, CountDownLatch* latch)
            : func_(func), name_(name), tid_(tid), latch_(latch)
        { 
        
        }

        // call gettid()
        void runInThread()
        {
            *tid_ = gettid();
            tid_ = NULL;
            latch_->countDown();
            latch_ = NULL;

            try {
                func_(); // starts a new thread in the calling process.
            }
            catch (const Exception& ex) {
                muduo::CurrentThread::t_threadName_ = "crashed";
                fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
                fprintf(stderr, "reason: %s\n", ex.what());
                fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
                abort();
            }
            catch (const std::exception& ex) {
                fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
                fprintf(stderr, "reason: %s\n", ex.what());
                abort();
            }
            catch (...) {
                fprintf(stderr, "unknown exception caught in Thread %s\n", name_.c_str());
                throw; // rethrow
            }
        }
    };

    // startThread() is invoked by Thread::start()
    void* startThread(void* obj)
    {
        ThreadData* data = static_cast<ThreadData*>(obj);
        data->runInThread();
        delete data;
        return NULL;
    }

} // namespace detail
} // namespace muduo

using namespace muduo;

std::atomic<int> Thread::numCreated_;

Thread::Thread(const ThreadFunc& func, const string& n) : started_(false), joined_(false), pthreadId_(0), tid_(0), func_(func), name_(n), latch_(1)
{
    setDefaultName(); // set default name for a thread
}

Thread::~Thread()
{
    if (started_ && !joined_) {
        pthread_detach(pthreadId_);
    }
}

void Thread::setDefaultName()
{
    int num = (++numCreated_);
    if (name_.empty()) {
        char buf[32];
        snprintf(buf, sizeof buf, "Thread%d", num);
        name_ = buf;
    }
}

// make threads ready to run and countdown
void Thread::start()
{
    assert(!started_);
    started_ = true;
    detail::ThreadData* data = new detail::ThreadData(func_, name_, &tid_, &latch_);
    /*
     * On success, pthread_create() returns 0; On error, it returns an error number.
     */
    if (pthread_create(&pthreadId_, NULL, &detail::startThread, data)) {
        started_ = false;
        delete data;
        //LOG_SYSFATAL << "Failed in pthread_create";
    } else {
        latch_.wait();
        assert(tid_ > 0);
    }
}

int Thread::join()
{
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthreadId_, NULL);
}

