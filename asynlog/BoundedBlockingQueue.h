#ifndef BOUNDEDBLOCKINGQUEUE_H
#define BOUNDEDBLOCKINGQUEUE_H

#include "Condition.h"
#include "Mutex.h"
#include "NonCopyable.h"
#include <assert.h>
#include <deque>
//#include <iostream>

namespace muduo
{

    template<typename T>
    class BoundedBlockingQueue : public noncopyable
    {
        public:
        explicit BoundedBlockingQueue(int maxSize) : mutex_(), notEmpty_(mutex_), notFull_(mutex_), queue_(maxSize), maxSize_(maxSize)
        {

        }

        void put(const T& x)
        {
            MutexLockGuard lock(mutex_);
            while (queue_.size() == maxSize_) {
                notFull_.wait();
            }
            assert(queue_.size() <= maxSize_);
            queue_.push_back(x);
            //std::cout << "put" << std::endl;
            notEmpty_.notify();
        }

        T take()
        {
            MutexLockGuard lock(mutex_);
            while (queue_.empty()) {
                notEmpty_.wait();
            }
            assert(!queue_.empty());
            T front(queue_.front());
            queue_.pop_front();
            //std::cout << "take" << std::endl;
            notFull_.notify();
            return front;
        }

        bool empty() const
        {
            MutexLockGuard lock(mutex_);
            return queue_.empty();
        }

        bool full() const
        {
            MutexLockGuard lock(mutex_);
            return queue_.size() == maxSize_;
        }

        size_t size() const
        {
            MutexLockGuard lock(mutex_);
            return queue_.size();
        }

        size_t capacity() const
        {
            MutexLockGuard lock(mutex_);
            return queue_.max_size();
        }

    private:
        mutable MutexLock mutex_;
        Condition notEmpty_;
        Condition notFull_;
        int maxSize_;
        std::deque<T>  queue_;
    };

}

#endif  // BOUNDEDBLOCKINGQUEUE_H
