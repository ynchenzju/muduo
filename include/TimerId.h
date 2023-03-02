//
// Created by Yunan Chen on 2023/1/10.
//

#ifndef MUDUO_TIMERID_H
#define MUDUO_TIMERID_H
#include "Timer.h"

class TimerId {
public:
    TimerId(): timer_(NULL), sequence_(0) {}

    TimerId(Timer* timer, int64_t seq)
            : timer_(timer),
              sequence_(seq) {}
    Timer* timer_;
    uint64_t sequence_;
};
#endif //MUDUO_TIMERID_H
