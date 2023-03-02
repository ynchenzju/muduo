//
// Created by Yunan Chen on 2023/1/4.
//

#ifndef THREADPOOL_EVENTLOOP_H
#define THREADPOOL_EVENTLOOP_H
#include "noncopyable.h"
#include "Poller.h"
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include "TimerId.h"
// #include "TimerQueue.h"
class TimerQueue;

class Channel;
class EventLoop : noncopyable{
public:
    typedef std::function<void()> Functor;
    EventLoop();
    ~EventLoop();
    void loop();

    void runInLoop(const Functor& cb);

    void assertInLoopThread() {
        if (!isInLoopThread()) {
            abortNotInLoopThread();
        }
    }

    bool isInLoopThread() const {
        return threadId_ == std::this_thread::get_id();
    }

    void queueInLoop(const Functor& cb);

    void quit();
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

    static EventLoop* getEventLoopOfCurrentThread();

    TimerId runAt(Timestamp time, TimerCallback cb);
    TimerId runAfter(double delay, TimerCallback cb);
    TimerId runEvery(double interval, TimerCallback cb);
private:
    typedef std::vector<Channel*> ChannelList;
    void abortNotInLoopThread();
    bool looping_;
    bool quit_;
    const std::thread::id threadId_;
    ChannelList activeChannels_;
    std::unique_ptr<Poller> poller_;
    std::unique_ptr<TimerQueue> timerQueue_;

    void handleRead();
    void doPendingFunctors();
    void wakeup();
    void handelRead();
    std::atomic<bool> callingPendingFunctors_;
    const int wakeupFd_;
    std::mutex mtx;
    std::unique_ptr<Channel> wakeupChannel_;
    std::vector<Functor> pendingFunctors_;
};
#endif //THREADPOOL_EVENTLOOP_H
