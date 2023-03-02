//
// Created by Yunan Chen on 2022/12/29.
//
#ifndef THREADPOOL_THREAD_H
#define THREADPOOL_THREAD_H
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <atomic>
#include "noncopyable.h"
#include "CountDownLatch.h"

class Thread : noncopyable
{
    public:
        typedef std::function<void ()> ThreadFunc;
        explicit Thread(ThreadFunc, const std::string& name = std::string());
        // FIXME: make it movable in C++11
        ~Thread();

        void start();
        void join(); // return pthread_join()

        bool started() const { return started_; }
        // pthread_t pthreadId() const { return pthreadId_; }
        std::thread::id tid() const { return std::this_thread::get_id(); }
        const std::string& name() const { return name_; }

        static int numCreated() { return numCreated_; }

    private:
        void setDefaultName();

        bool       started_;
        bool       joined_;
        // pthread_t  pthreadId_;
        std::thread::id tid_;
        ThreadFunc func_;
        std::string name_;
        CountDownLatch latch_;
        std::thread thd;

        static std::atomic<int> numCreated_;
};

struct ThreadData {
    typedef std::function<void ()> ThreadFunc;
    ThreadFunc func_;
    std::string name_;
    std::thread::id tid_;
    CountDownLatch* latch_;

    ThreadData(ThreadFunc func,
               const std::string& name,
               std::thread::id& tid,
               CountDownLatch* latch) : func_(std::move(func)),
               name_(name),
               tid_(tid),
               latch_(latch) {}

    void runInThread() {
        latch_->countDown();
        latch_ = NULL;

        try {
            func_();
        } catch (...) {
            fprintf(stderr, "unknown exception caught in Thread %s\n", name_.c_str());
            throw;
        }
    }
};

#endif //THREADPOOL_THREAD_H
