//
// Created by Yunan Chen on 2023/1/4.
//

#ifndef THREADPOOL_CHANNEL_H
#define THREADPOOL_CHANNEL_H
#include "noncopyable.h"
//#include "EventLoop.h"
#include "TimeStamp.h"
#include <functional>
#include <memory>
class EventLoop;

class Channel : noncopyable {
public:
    typedef std::function<void()> EventCallback;
    typedef std::function<void(Timestamp)> ReadEventCallback;
    Channel(EventLoop *loop, int fd);
    ~Channel();
    void handleEvent();

//    void setReadCallback(const EventCallback& cb) {
//        readCallback_ = cb;
//    }
    void setReadCallback(const EventCallback& cb) {
        readCallback_ = cb;
    }
    void setWriteCallback(const EventCallback& cb) {
        writeCallback_ = cb;
    }
    void setErrorCallback(const EventCallback& cb) {
        errorCallback_ = cb;
    }
    void setCloseCallback(const EventCallback& cb) {
        closeCallback_ = cb;
    }

    void tie(const std::shared_ptr<void>&);

    int fd() const {return fd_;}
    int events() const { return events_;}
    void set_revents(int revt) {revents_ = revt;}
    bool isNoneEvent() const {return events_ == kNoneEvent;}
    void enableReading() {events_ |= kReadEvent; update();}
    void disableAll() { events_ = kNoneEvent; update(); }
    void enableWriting() { events_ |= kWriteEvent; update();}
    void disableWriting() { events_ &= ~kWriteEvent; update();}
    bool isWriting() const { return events_ & kWriteEvent;}
    int index() {return index_;}
    void set_index(int idx) {index_ = idx;}
    EventLoop* ownerLoop() {return loop_;}
    void remove();

private:
    std::weak_ptr<void> tie_;
    bool tied_;

    void update();
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* loop_;
    const int fd_;
    int events_;
    int revents_;
    int index_;

    bool eventHandling_;

//    EventCallback readCallback_;
    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
    EventCallback closeCallback_;
};
#endif //THREADPOOL_CHANNEL_H














