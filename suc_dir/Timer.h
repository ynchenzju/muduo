//
// Created by Yunan Chen on 2023/1/10.
//

#ifndef MUDUO_TIMER_H
#define MUDUO_TIMER_H
#include "noncopyable.h"
#include "TimeStamp.h"
#include <functional>
#include <atomic>

typedef std::function<void()> TimerCallback;
class Timer : noncopyable {

public:

    Timer(TimerCallback cb, Timestamp when, double interval) : callback_(std::move(cb)),
        expiration_(when), interval_(interval), repeat_(interval > 0.0), sequence_(s_numCreated_++) {}

    void run() const {
        callback_();
    }

    Timestamp expiration() const { return expiration_;}
    bool repeat() const { return repeat_; }
    uint64_t sequence() const { return sequence_; }
    void restart(Timestamp now);
    static uint64_t numCreated() { return s_numCreated_; }
private:
    const TimerCallback callback_;
    Timestamp expiration_;
    const double interval_;
    const bool repeat_;
    const int64_t sequence_;
    static std::atomic<uint64_t> s_numCreated_;
};

#endif //MUDUO_TIMER_H
