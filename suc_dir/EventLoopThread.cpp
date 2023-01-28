//
// Created by Yunan Chen on 2023/1/18.
//
#include "EventLoop.h"
#include "EventLoopThread.h"
#include <cassert>

EventLoopThread::EventLoopThread(const std::string &name) : loop_(NULL),
    exiting_(false), thread_(std::bind(&EventLoopThread::threadFunc, this), name) {
}

EventLoopThread::~EventLoopThread() {
    exiting_ = true;
    // not 100% race-free, eg. threadFunc could be running callback_.
    if (loop_ != NULL) {
        // still a tiny chance to call destructed object, if threadFunc exits just now.
        // but when EventLoopThread destructs, usually programming is exiting anyway.
        loop_->quit();
        thread_.join();
    }
}

EventLoop* EventLoopThread::startLoop() {
    assert(!thread_.started());
    thread_.start();
    EventLoop* loop = NULL;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while (loop_ == NULL) {
            cond_.wait(lock);
        }
        loop = loop_;
    }
    return loop;
}

void EventLoopThread::threadFunc() {
    EventLoop loop;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = &loop;
        cond_.notify_all();
    }
    loop.loop();
    std::unique_lock<std::mutex> lock(mutex_);
    loop_ = NULL;
}

