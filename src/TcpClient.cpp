//
// Created by Yunan Chen on 2023/2/27.
//
#include "TcpClient.h"
#include "Connector.h"
#include "TimeStamp.h"
#include "EventLoop.h"

void removeConnection2(EventLoop* loop, const TcpConnectionPtr& conn)
{
    loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}

void defaultConnectionCallback(const TcpConnectionPtr& conn)
{
//    LOG_TRACE << conn->localAddress().toIpPort() << " -> "
//              << conn->peerAddress().toIpPort() << " is "
//              << (conn->connected() ? "UP" : "DOWN");
    // do not call conn->forceClose(), because some users want to register message callback only.
}

void defaultMessageCallback(const TcpConnectionPtr&, Buffer* buf, Timestamp) {
    buf->retrieveAll();
}

TcpClient::TcpClient(EventLoop* loop, const InetAddress& serverAddr, const std::string& nameArg) : loop_(loop),
    connector_(new Connector(loop, serverAddr)), name_(nameArg), connectionCallback_(defaultConnectionCallback),
    messageCallback_(defaultMessageCallback), retry_(false), connect_(false), nextConnId_(1) {
    connector_->setNewConnectionCallback(std::bind(&TcpClient::newConnection, this, std::placeholders::_1));
}


TcpClient::~TcpClient() {
    TcpConnectionPtr conn;
    bool unique = false;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        unique = connection_.unique();
        conn = connection_;
    }
    if (conn) {
        assert(loop_ == conn->getLoop());
        // FIXME: not 100% safe, if we are in different thread
        CloseCallback cb = std::bind(&removeConnection2, loop_, std::placeholders::_1);
        loop_->runInLoop(
                std::bind(&TcpConnection::setCloseCallback, conn, cb));
        if (unique) {
            conn->forceClose();
        }
    }
}


void TcpClient::connect() {
    // FIXME: check state
//    LOG_INFO << "TcpClient::connect[" << name_ << "] - connecting to "
//             << connector_->serverAddress().toIpPort();
    connect_ = true;
    connector_->start();
}


void TcpClient::disconnect() {
    connect_ = false;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (connection_) {
            connection_->shutdown();
        }
    }
}

void TcpClient::stop() {
    connect_ = false;
    connector_->stop();
}


void TcpClient::newConnection(int sockfd) {
    loop_->assertInLoopThread();
    InetAddress peerAddr(sockets::getPeerAddr(sockfd));
    char buf[32];
    snprintf(buf, sizeof buf, ":%s#%d", peerAddr.toIpPort().c_str(), nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;

    InetAddress localAddr(sockets::getLocalAddr(sockfd));
    // FIXME poll with zero timeout to double confirm the new connection
    // FIXME use make_shared if necessary
    TcpConnectionPtr conn(new TcpConnection(loop_,
                                            connName,
                                            sockfd,
                                            localAddr,
                                            peerAddr));

    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(
            std::bind(&TcpClient::removeConnection, this, std::placeholders::_1)); // FIXME: unsafe
    {
        std::unique_lock<std::mutex> lock(mutex_);
        connection_ = conn;
    }
    conn->connectEstablished();
}

void TcpClient::removeConnection(const TcpConnectionPtr& conn) {
    loop_->assertInLoopThread();
    assert(loop_ == conn->getLoop());

    {
        std::unique_lock<std::mutex> lock(mutex_);
        assert(connection_ == conn);
        connection_.reset();
    }

    loop_->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    if (retry_ && connect_)
    {
//        LOG_INFO << "TcpClient::connect[" << name_ << "] - Reconnecting to "
//                 << connector_->serverAddress().toIpPort();
        connector_->restart();
    }
}
