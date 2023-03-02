//
// Created by Yunan Chen on 2022/12/29.
//

#ifndef THREADPOOL_MUTEXLOCK_H
#define THREADPOOL_MUTEXLOCK_H
#include <mutex>
#include "noncopyable.h"
class MutexLock : noncopyable {
private:
    std::mutex mtx;

public:
    void lock() {
        mtx.lock();
    }
    void unlock() {
        mtx.unlock();
    }
};

class MutexLockGuard : noncopyable {
private:
    MutexLock& mutex_;
public:
    explicit MutexLockGuard(MutexLock& mutex) : mutex_(mutex) {
        mutex_.lock();
    }
    ~MutexLockGuard() {
        mutex_.unlock();
    }
};
#endif //THREADPOOL_MUTEXLOCK_H
