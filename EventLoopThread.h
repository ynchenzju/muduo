//
// Created by Yunan Chen on 2023/1/18.
//

#ifndef MUDUO_EVENTLOOPTHREAD_H
#define MUDUO_EVENTLOOPTHREAD_H
#include "noncopyable.h"
#include <functional>
#include <string>
#include "Thread.h"

class EventLoop;
class EventLoopThread : noncopyable {
public:
    typedef std::function<void(EventLoop*)> ThreadInitCallback;
    EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(),
                    const std::string& name = std::string());
    ~EventLoopThread();
    EventLoop* startLoop();

private:
    void threadFunc();
    EventLoop* loop_;
    bool exiting_;
    Thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
    ThreadInitCallback callback_;
};

#endif //MUDUO_EVENTLOOPTHREAD_H
