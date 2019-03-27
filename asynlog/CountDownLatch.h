#ifndef COUNTDOWNLATCH_H
#define COUNTDOWNLATCH_H

#include "Condition.h"
#include "Mutex.h"
#include "NonCopyable.h"

/*
 * CountDownLatch是通过一个计数器来实现的，计数器的初始值为线程的数量。
 * 每当一个线程完成了自己的任务后，计数器的值就会减1。当计数器值到达0时，
 * 它表示所有的线程已经完成了任务，然后在闭锁上等待的线程就可以恢复执行任务。
 */

namespace muduo
{

class CountDownLatch : public noncopyable
{
    public:
        explicit CountDownLatch(int count);
        void wait();
        void countDown();
        int getCount() const;

    private:
        mutable MutexLock mutex_;
        Condition condition_;
        int count_;
};

} // namespace muduo
#endif  // COUNTDOWNLATCH_H
