//
// Created by Yunan Chen on 2023/1/4.
//

#ifndef THREADPOOL_EVENTLOOP_H
#define THREADPOOL_EVENTLOOP_H
#include "noncopyable.h"
#include "Poller.h"
#include "Timerfd.h"
#include <thread>
#include <vector>
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
private:
    typedef std::vector<Channel*> ChannelList;
    void abortNotInLoopThread();
    bool looping_;
    bool quit_;
    const std::thread::id threadId_;
    ChannelList activeChannels_;
    std::unique_ptr<Poller> poller_;

    void handleRead();
    void doPendingFunctors();
    void wakeup();
    void handelRead();
    std::atomic<bool> callingPendingFunctors_;
    Timerfd wakeupFd_;
    std::mutex mtx;
    std::unique_ptr<Channel> wakeupChannel_;
    std::vector<Functor > pendingFunctors_;
};
#endif //THREADPOOL_EVENTLOOP_H
