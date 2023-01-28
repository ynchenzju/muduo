//
// Created by Yunan Chen on 2022/12/29.
//
#include "Thread.h"
#include <pthread.h>
#include <cassert>

void startThread(void* obj)
{
    ThreadData* data = static_cast<ThreadData*>(obj);
    data->runInThread();
    delete data;
}

std::atomic<int> Thread::numCreated_;
typedef std::function<void ()> ThreadFunc;

Thread::Thread(ThreadFunc func, const std::string& name): latch_(1),
                    func_(std::move(func)),
                    name_(name), started_(false), joined_(false) { setDefaultName(); };

void Thread::setDefaultName()
{
    // int num = numCreated_.incrementAndGet();
    ++numCreated_;
    int num = numCreated_.load();
    if (name_.empty())
    {
        char buf[32];
        snprintf(buf, sizeof buf, "Thread%d", num);
        name_ = buf;
    }
}


void Thread::start()
{
    assert(!started_);
    started_ = true;
    // FIXME: move(func_)
    ThreadData* data = new ThreadData(func_, name_, tid_, &latch_);
    thd = std::thread(startThread, data);
    latch_.wait();
}

void Thread::join()
{
    assert(started_);
    assert(!joined_);
    joined_ = true;
    thd.join();
}

Thread::~Thread() {
    if (started_ && !joined_) {
        thd.detach();
    }
}
