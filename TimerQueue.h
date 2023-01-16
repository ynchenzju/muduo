//
// Created by Yunan Chen on 2023/1/10.
//

#ifndef MUDUO_TIMERQUEUE_H
#define MUDUO_TIMERQUEUE_H
#include "noncopyable.h"
#include "TimeStamp.h"
#include "Timer.h"
#include "TimerId.h"
#include "EventLoop.h"
#include "Timerfd.h"
#include <set>
#define UINTPTR_MAX    18446744073709551615UL

class TimerQueue : noncopyable {
public:
    explicit TimerQueue(EventLoop* loop);
    ~TimerQueue();
    TimerId addTimer(TimerCallback cb, Timestamp when, double interval);
    void cancel(TimerId timerId);

private:
    typedef std::pair<Timestamp, Timer*> Entry;
    typedef std::set<Entry> TimerList;
    // typedef std::set<std::pair<Timestamp, Timer*> > TimerList;
    typedef std::pair<Timer*, int64_t> ActiveTimer;
    typedef std::set<ActiveTimer> ActiveTimerSet;
    void addTimerInLoop(Timer* timer);
    void cancelInLoop(TimerId timerId);
    void handleRead();
    std::vector<Entry> getExpired(Timestamp now);
    void reset(const std::vector<Entry>& expired, Timestamp now);
    void resetTimerfd(int timerfd, Timestamp expiration);

    bool insert(Timer* timer);

    EventLoop* loop_;
    Timerfd timerfd_;
    Channel timerfdChannel_;
    TimerList timers_;

    ActiveTimerSet  activeTimers_;
    std::atomic<bool> callingExpiredTimers_;
    ActiveTimerSet cancelingTimers_;
};
#endif //MUDUO_TIMERQUEUE_H
