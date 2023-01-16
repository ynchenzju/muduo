//
// Created by Yunan Chen on 2022/12/31.
//

#ifndef THREADPOOL_THREADPOOL_H
#define THREADPOOL_THREADPOOL_H
#include "noncopyable.h"
#include "Thread.h"
#include <mutex>
#include <deque>
#include <vector>

class ThreadPool : noncopyable {
public:
    typedef std::function<void()> Task;

    explicit ThreadPool(const std::string& nameArg = std::string("ThreadPool"));
    ~ThreadPool();
    void setMaxQueueSize(int maxSize) { maxQueueSize_ = maxSize; }
    void start(int numThreads);
    void stop();
    void run(Task f);
    size_t queueSize() const;
    Task take();
    void runInThread();
    bool isFull() const;

private:
    std::string name_;
    mutable std::mutex mutex_;
    std::condition_variable notEmpty_;
    std::condition_variable notFull_;
    std::deque<Task> queue_;
    size_t maxQueueSize_;
    std::vector<std::unique_ptr<Thread>> threads_;
    bool running_;
};


#endif //THREADPOOL_THREADPOOL_H
