//
// Created by Yunan Chen on 2023/1/4.
//

#ifndef THREADPOOL_POLLER_H
#define THREADPOOL_POLLER_H

#include <vector>
#include <map>
#include "noncopyable.h"
#include <sys/poll.h>

class Channel;
// struct pollfd;
class EventLoop;

class Poller : noncopyable {
public:
    typedef std::vector<Channel*> ChannelList;
    Poller(EventLoop* loop);
    ~Poller();
    // Polls the I/O events
    // Must be called in the loop thread
    void poll(int timeoutMs, ChannelList* activeChannels);

    // changes the interested I/O events;
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

    void assertInLoopThread();
private:
    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;
    typedef std::vector<struct pollfd> PollFdList;
    typedef std::map<int, Channel*> ChannelMap;
    EventLoop* ownerLoop_;
    PollFdList pollfds_;
    ChannelMap channels_;
};

#endif //THREADPOOL_POLLER_H
