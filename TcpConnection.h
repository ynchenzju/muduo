//
// Created by Yunan Chen on 2023/2/18.
//

#ifndef MUDUO_TCPCONNECTION_H
#define MUDUO_TCPCONNECTION_H
#include <memory>
#include <string>
#include <functional>
#include <functional>
#include "noncopyable.h"
#include "InetAddress.h"
#include "Buffer.h"
class Socket;
class Channel;

class Timestamp;
class TcpConnection;
class EventLoop;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void (const TcpConnectionPtr&,
                            Buffer*,
                            Timestamp)> MessageCallback;
typedef std::function<void (const TcpConnectionPtr&)> CloseCallback;
typedef std::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;


class TcpConnection : noncopyable, public std::enable_shared_from_this<TcpConnection> {
public:
    TcpConnection(EventLoop* loop,
                  const std::string& name,
                  int sockfd,
                  const InetAddress& localAddr,
                  const InetAddress& peerAddr);
    ~TcpConnection();
    void setCloseCallback(const CloseCallback& cb) {
        closeCallback_ = cb;
    }
    void setConnectionCallback(const ConnectionCallback& cb)
    { connectionCallback_ = cb; }

    void setMessageCallback(const MessageCallback& cb)
    { messageCallback_ = cb; }

    void setWriteCompleteCallback(const WriteCompleteCallback& cb)
    { writeCompleteCallback_ = cb; }

    EventLoop* getLoop() const { return loop_; }

    const std::string& name() const { return name_; }

    void send(const std::string& message);
    void shutdown();
    void setTcpNoDelay(bool on);

    void connectEstablished();
    void connectDestroyed();

    void forceClose();
    void forceCloseInLoop();

private:
    enum StateE { kConnecting, kConnected, kDisconnecting, kDisconnected,};
    void setState(StateE s) { state_ = s; }
    void handleRead(Timestamp receiveTime);
    void handleWrite();
    void handleClose();
    void handleError();
    void sendInLoop(const std::string& message);
    void shutdownInLoop();

    bool reading_;
    EventLoop* loop_;
    std::string name_;
    StateE state_;
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;
    InetAddress localAddr_;
    InetAddress peerAddr_;
    ConnectionCallback connectionCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    MessageCallback messageCallback_;
    CloseCallback closeCallback_;
    Buffer inputBuffer_;
    Buffer outputBuffer_;
};

#endif //MUDUO_TCPCONNECTION_H
