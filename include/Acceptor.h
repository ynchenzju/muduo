//
// Created by Yunan Chen on 2023/2/13.
//

#ifndef MUDUO_ACCEPTOR_H
#define MUDUO_ACCEPTOR_H
#include "Channel.h"
#include "noncopyable.h"
#include "Socket.h"

class EventLoop;
class InetAddress;

class Acceptor : noncopyable {
public:
    typedef std::function<void(int sockfd, const InetAddress&)> NewConnectionCallback;
    Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback& cb) { newConnectionCallback_ = cb; }
    void listen();
    bool listening() const { return listening_; }

private:
    void handleRead();
    EventLoop* loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    bool listening_;
    int idleFd_;
    NewConnectionCallback newConnectionCallback_;
};

#endif //MUDUO_ACCEPTOR_H
