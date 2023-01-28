//
// Created by Yunan Chen on 2023/1/13.
//

#ifndef MUDUO_TIMERFD_H
#define MUDUO_TIMERFD_H
#include "noncopyable.h"
#include "poll.h"
#include <unistd.h>
#include <thread>

class Timerfd : public noncopyable {
public:
    int _pipe[2];
    bool _is_timing;
    Timerfd();
    ~Timerfd();
    int OpenFd();
    int GetFd();

    static void SetTimer(Timerfd* tfd, int ms) {
        if (tfd->_is_timing) {
            return;
        }
        std::thread timer([tfd, ms]() {
            ::poll(nullptr, 0, ms);
            ::write(tfd->_pipe[1], "", 1);
        });
        timer.detach();
    }
};
#endif //MUDUO_TIMERFD_H
