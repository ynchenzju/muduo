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
#include <atomic>

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
    typedef std::function<void(EventLoop*)> ThreadInitCallback;
    enum Option {
        kNoReusePort,
        kReusePort,
    };
    TcpServer(EventLoop* loop, const InetAddress& listenAddr);
    TcpServer(EventLoop* loop,
              const InetAddress& listenAddr,
              const std::string& nameArg,
              Option option = kNoReusePort);
    ~TcpServer();
    void setThreadNum(int numThreads);
    void start();

    void setConnectionCallback(const ConnectionCallback& cb) {
        connectionCallback_ = cb;
    }
    void setMessageCallback(const MessageCallback& cb) {
        messageCallback_ = cb;
    }

    void setThreadInitCallback(const ThreadInitCallback& cb)
    { threadInitCallback_ = cb; }

    void removeConnection(const TcpConnectionPtr& conn);

    const std::string& name() const { return name_; }

private:
    void removeConnectionInLoop(const TcpConnectionPtr& conn);
    void newConnection(int sockfd, const InetAddress& peerAddr);
    EventLoop* loop_;
    const std::string name_;
    std::unique_ptr<Acceptor> acceptor_;
//    bool started_;
//    std::atomic<bool> started_;
    std::atomic_flag started_;
    int nextConnId_;
    std::unique_ptr<EventLoopThreadPool> threadPool_;

    ConnectionCallback  connectionCallback_;
    MessageCallback messageCallback_;
    ThreadInitCallback threadInitCallback_;
    ConnectionMap  connections_;
    const std::string ipPort_;
};

#endif //MUDUO_TCPSERVER_H
