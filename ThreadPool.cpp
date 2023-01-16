//
// Created by Yunan Chen on 2022/12/31.
//
#include "ThreadPool.h"

ThreadPool::ThreadPool(const std::string &nameArg) : name_(nameArg), maxQueueSize_(0), running_(false) {}

ThreadPool::~ThreadPool() {
    if (running_) {
        stop();
    }
}

void ThreadPool::stop() {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        running_ = false;
        notEmpty_.notify_all();
        notFull_.notify_all();
    }

    for (auto& thr : threads_ ) {
        thr->join();
    }
}

void ThreadPool::runInThread() {
    try {
        while (running_) {
            Task task(take());
            if (task) {
                task();
            }
        }
    } catch (...) {

    }
}

void ThreadPool::start(int numThreads) {
    assert(threads_.empty());
    running_ = true;
    threads_.reserve(numThreads);
    for (int i = 0; i < numThreads; ++i) {
        char id[32];
        snprintf(id, sizeof id, "%d", i+1);
        threads_.emplace_back(new Thread(std::bind(&ThreadPool::runInThread, this), name_ + id));
        // start里面，ThreadPool线程会被阻塞，等待子线程启动完毕
        threads_[i]->start();
    }
}

ThreadPool::Task ThreadPool::take() {
    std::unique_lock<std::mutex> lock(mutex_);
    // 为啥要循环？防止虚假唤醒
    while (queue_.empty() && running_) {
        notEmpty_.wait(lock);
    }
    Task task;
    // 这里为什么不用判断running_状态？
    // 线程等在notEmpty_上意味着此时queue是空
    // 且如果stop了，running_ = false，那就不会往queue里塞任务
    // 那此时queue_.empty() == true, 不会执行if里的语句
    if (!queue_.empty()) {
        task = queue_.front();
        queue_.pop_front();
        if (maxQueueSize_ > 0) {
            notFull_.notify_one();
        }
    }
    return task;
}

void ThreadPool::run(Task task) {
    if (threads_.empty()) {
        task();
    } else {
        // 在queue没有满的时候，不会阻塞
        // queue满的时候，阻塞是有道理的，意味着机器资源用完，必须等待
        std::unique_lock<std::mutex> lock(mutex_);
        while (isFull() && running_) {
            notFull_.wait(lock);
        }
        // 如果stop了，不能往queue里塞任务，直接返回
        if (!running_) {
            return;
        }
        assert(!isFull());
        queue_.push_back(std::move(task));
        notEmpty_.notify_one();
    }
}

size_t ThreadPool::queueSize() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return queue_.size();
}

bool ThreadPool::isFull() const
{
    // mutex_.assertLocked();
    return maxQueueSize_ > 0 && queue_.size() >= maxQueueSize_;
}