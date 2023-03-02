//
// Created by Yunan Chen on 2023/2/27.
//

#ifndef MUDUO_TCPCLIENT_H
#define MUDUO_TCPCLIENT_H

#include "TcpConnection.h"
#include "noncopyable.h"
#include "InetAddress.h"
#include <string>
#include <atomic>
#include <mutex>

class Connector;
typedef std::shared_ptr<Connector> ConnectorPtr;

class TcpClient : noncopyable {
    TcpClient(EventLoop* loop,
              const InetAddress& serverAddr,
              const std::string& nameArg);
    ~TcpClient();  // force out-line dtor, for std::unique_ptr members.

    void connect();
    void disconnect();
    void stop();
private:
    void newConnection(int sockfd);
    void removeConnection(const TcpConnectionPtr& conn);
    EventLoop* loop_;
    ConnectorPtr connector_;
    const std::string name_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;

    std::atomic<bool> retry_;
    std::atomic<bool> connect_;
    int nextConnId_;
    mutable std::mutex mutex_;
    TcpConnectionPtr connection_;
};


#endif //MUDUO_TCPCLIENT_H
