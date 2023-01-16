cmake_minimum_required(VERSION 3.23)
project(muduo)

set(CMAKE_CXX_STANDARD 20)

add_executable(muduo main.cpp CountDownLatch.h noncopyable.h MutexLock.h Condition.h Thread.h Thread.cpp ThreadPool.h ThreadPool.cpp Channel.h Channel.cpp Poller.h Poller.cpp EventLoop.h EventLoop.cpp TimeStamp.h Timer.h TimerId.h TimerQueue.h TimerQueue.cpp Timerfd.cpp Timerfd.h)
