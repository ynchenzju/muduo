cmake_minimum_required(VERSION 3.16)
project(muduo)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

add_executable(muduo main.cpp CountDownLatch.h noncopyable.h MutexLock.h Condition.h Thread.h Thread.cpp ThreadPool.h ThreadPool.cpp Channel.h Channel.cpp Poller.h Poller.cpp EventLoop.h EventLoop.cpp TimeStamp.h Timer.h TimerId.h TimerQueue.h TimerQueue.cpp Timerfd.cpp Timerfd.h EventLoopThread.cpp EventLoopThread.h EventLoopThread.cpp TimeStamp.cpp Timer.cpp)
target_link_libraries(muduo Threads::Threads)
