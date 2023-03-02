//
// Created by Yunan Chen on 2023/1/4.
//

/*
poll
跟select功能类似，可以设置的同时监听上限会更多,poll效率更高,调用完poll函数之后不会清空监听的事件集合.
int poll(struct pollfd *fds, nfds_t nfds, int timeout);
-fds: 是一个struct pollfd结构类型的数组，用于存放需要检测其状态的socket描述符。结构体类型定义如下：
struct pollfd {
    int fd;	//文件描述符
    short events;	//等待的需要监听的事件类型,常用取值为POLLIN(监听读)/POLLOUT(写)/POLLERR(异常)。如fds[0].events = POLLIN
    short revents;	//实际发生了的事件，也就是返回结果。值的范围同events: POLLIN/POLLOUT/POLLERR
};

-nfds: nfds_t类型的参数，用于标记数组fds中的结构体元素的总数量；
-timeout: 是poll函数调用阻塞的时间，单位：毫秒。传值-1表示阻塞监听，0表示不阻塞立即返回，>0表示阻塞等待timeout的时间

返回值：>0：数组fds中准备好读、写或出错状态的那些socket描述符的总数量，fds数组中有状态的fd的revents被赋值传出，可以通过跟POLLIN/POLLOUT/POLLERR等标志通过位与&来判断，如if(fds[n].revents & POLLIN)
=0:数组fds中没有任何socket描述符准备好读、写，或出错，revents会被清空
=-1：poll函数调用失败，同时会自动设置全局变量errno.
 */
#include "Channel.h"
#include "EventLoop.h"
#include <sys/poll.h>
#include <cassert>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI; // 监听读事件
const int Channel::kWriteEvent = POLLOUT; // 监听写事件

Channel::Channel(EventLoop *loop, int fdArg): loop_(loop), fd_(fdArg), events_(0), revents_(0), index_(-1), tied_(false) {}

void Channel::update() {
    loop_->updateChannel(this);
}

void Channel::handleEvent() {
    eventHandling_ = true;
    if (revents_ & POLLNVAL) {

    }

    if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
        if (closeCallback_) {
            closeCallback_();
        }
    }

    if (revents_ & (POLLERR | POLLNVAL)) {
        if (errorCallback_) {
            errorCallback_();
        }
    }

    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
        if (readCallback_) {
            readCallback_();
        }
    }
    if (revents_ & POLLOUT) {
        if (writeCallback_) {
            writeCallback_();
        }
    }
    eventHandling_ = false;
}

void Channel::remove() {
    loop_->removeChannel(this);
}

Channel::~Channel() {
    assert(!eventHandling_);
}

void Channel::tie(const std::shared_ptr<void>& obj)
{
    tie_ = obj;
    tied_ = true;
}