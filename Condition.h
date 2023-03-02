//
// Created by Yunan Chen on 2022/12/29.
//

#ifndef THREADPOOL_CONDITION_H
#define THREADPOOL_CONDITION_H
#include "MutexLock.h"
#include "noncopyable.h"
#include <condition_variable>
#include <mutex>

class Condition : noncopyable {
public:
    explicit Condition(std::mutex& mutex) : mutex_(mutex) {}

    ~Condition() {}
    void wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        pcond_.wait(lock);
    }
    void notify() {
        pcond_.notify_one();
    }

    void notifyAll() {
        pcond_.notify_all();
    }

private:
    // MutexLock& mutex_;
    std::mutex& mutex_;
    std::condition_variable pcond_;
};

#endif //THREADPOOL_CONDITION_H
