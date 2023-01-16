//
// Created by Yunan Chen on 2022/12/29.
//

#ifndef THREADPOOL_COUNTDOWNLATCH_H
#define THREADPOOL_COUNTDOWNLATCH_H
#include "noncopyable.h"
#include "MutexLock.h"
#include "Condition.h"
class CountDownLatch : noncopyable {
public:
    explicit CountDownLatch(int count) : count_(count) {};
    void wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        while (count_ > 0) {
            pcond_.wait(lock);
        }
    };
    void countDown() {
        std::unique_lock<std::mutex> lock(mutex_);
        --count_;
        if (count_ == 0) {
            pcond_.notify_all();
        }
    };
    int getCount() const {
        std::unique_lock<std::mutex> lock(mutex_);
        return count_;
    };

private:
    mutable std::mutex mutex_; // 加mutable修饰是因为要在getCount中加锁
    std::condition_variable pcond_;
    int count_;
};

#endif //THREADPOOL_COUNTDOWNLATCH_H
