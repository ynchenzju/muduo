//
// Created by Yunan Chen on 2023/1/13.
//
#include "Timerfd.h"

Timerfd::~Timerfd() {
    close(_pipe[0]);
    close(_pipe[1]);
}

Timerfd::Timerfd() : _is_timing(false) {
    if (pipe(_pipe) != 0) {

    }
}

int Timerfd::GetFd() {
    return _pipe[0];
}

