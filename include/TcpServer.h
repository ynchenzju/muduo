//
// Created by Yunan Chen on 2023/2/18.
//

#ifndef MUDUO_TCPSERVER_H
#define MUDUO_TCPSERVER_H

#include "Acceptor.h"
#include "noncopyable.h"
#include <functional>
#include <memory>
#include <string>
#include <map>

class EventLoop;
class InetAddress;
class TcpConnection;
class EventLoopThreadPool;
class Buffer;
class Timestamp;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void (const TcpConnectionPtr&,
                            Buffer*,
                            Timestamp)> MessageCallback;

typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

class TcpServer: noncopyable {
public:
    TcpServer(EventLoop* loop, const InetAddress& listenAddr);
    ~TcpServer();
    void setThreadNum(int numThreads);
    void start();

    void setConnectionCallback(const ConnectionCallback& cb) {
        connectionCallback_ = cb;
    }
    void setMessageCallback(const MessageCallback& cb) {
        messageCallback_ = cb;
    }

    void removeConnection(const TcpConnectionPtr& conn);

    const std::string& name() const { return name_; }

private:
    void removeConnectionInLoop(const TcpConnectionPtr& conn);
    void newConnection(int sockfd, const InetAddress& peerAddr);
    EventLoop* loop_;
    const std::string name_;
    std::unique_ptr<Acceptor> acceptor_;
    bool started_;
    int nextConnId_;
    std::unique_ptr<EventLoopThreadPool> threadPool_;

    ConnectionCallback  connectionCallback_;
    MessageCallback messageCallback_;
    ConnectionMap  connections_;
};

#endif //MUDUO_TCPSERVER_H
