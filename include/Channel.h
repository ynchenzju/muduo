//
// Created by Yunan Chen on 2023/1/4.
//

#ifndef THREADPOOL_CHANNEL_H
#define THREADPOOL_CHANNEL_H
#include "noncopyable.h"
//#include "EventLoop.h"
#include <functional>
class EventLoop;

class Channel : noncopyable {
public:
    typedef std::function<void()> EventCallback;
    Channel(EventLoop *loop, int fd);
    ~Channel();
    void handleEvent();

    void setReadCallback(const EventCallback& cb) {
        readCallback_ = cb;
    }
    void setWriteCallback(const EventCallback& cb) {
        writeCallback_ = cb;
    }
    void setErrorCallback(const EventCallback& cb) {
        errorCallback_ = cb;
    }
    int fd() const {return fd_;}
    int events() const { return events_;}
    void set_revents(int revt) {revents_ = revt;}
    bool isNoneEvent() const {return events_ == kNoneEvent;}
    void enableReading() {events_ |= kReadEvent; update();}
    void disableAll() { events_ = kNoneEvent; update(); }
    int index() {return index_;}
    void set_index(int idx) {index_ = idx;}
    EventLoop* ownerLoop() {return loop_;}
    void remove();

private:
    void update();
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* loop_;
    const int fd_;
    int events_;
    int revents_;
    int index_;

    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
};
#endif //THREADPOOL_CHANNEL_H














