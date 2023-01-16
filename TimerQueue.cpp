//
// Created by Yunan Chen on 2023/1/11.
//
#include "TimerQueue.h"
#define UINTPTR_MAX  18446744073709551615UL
TimerQueue::TimerQueue(EventLoop *loop) : loop_(loop),
    timerfd_(), timerfdChannel_(loop, timerfd_.GetFd()),
    callingExpiredTimers_(false) {}

TimerQueue::~TimerQueue() {
    timerfdChannel_.disableAll();
    timerfdChannel_.remove();
    for (const Entry& timer: timers_) {
        delete timer.second;
    }
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now) {
    std::vector<Entry> expired;
    Entry sentry = std::make_pair(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    TimerList::iterator it = timers_.lower_bound(sentry);
    assert(it == timers_.end() || now < it->first);
    std::copy(timers_.begin(), it, std::back_inserter(expired));
    timers_.erase(timers_.begin(), it);
    return expired;
}

TimerId TimerQueue::addTimer(TimerCallback cb, Timestamp when, double interval) {
    Timer* timer = new Timer(std::move(cb), when, interval);
    loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
    return TimerId(timer, timer->sequence());
}

void TimerQueue::addTimerInLoop(Timer *timer) {
    loop_->assertInLoopThread();
    bool earliestChanged = insert(timer);
    if (earliestChanged) {
        resetTimerfd(timerfd_.GetFd(), timer->expiration());
    }
}

void TimerQueue::resetTimerfd(int timerfd, Timestamp expiration) {

}

bool TimerQueue::insert(Timer *timer) {
    loop_->assertInLoopThread();
    bool earliestChanged = false;
    Timestamp when = timer->expiration();
    TimerList::iterator it = timers_.begin();
    if (it == timers_.end() || when < it->first) {
        earliestChanged = true;
    }
    {
        std::pair<TimerList::iterator, bool> result = timers_.insert(Entry(when, timer));
        assert(result.second);
    }
    activeTimers_.insert(ActiveTimer(timer, timer->sequence()));
    return earliestChanged;
}







