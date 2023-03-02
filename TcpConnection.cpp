//
// Created by Yunan Chen on 2023/2/18.
//

#include "TcpConnection.h"
#include <unistd.h>
#include "Channel.h"
#include "EventLoop.h"
#include <cassert>
#include <functional>
#include "../include/SocketsOps.h"
#include "Socket.h"


TcpConnection::TcpConnection(EventLoop* loop,
                             const std::string& nameArg,
                             int sockfd,
                             const InetAddress& localAddr,
                             const InetAddress& peerAddr)
        : loop_(loop),
          name_(nameArg),
          state_(kConnecting),
          reading_(true),
          socket_(new Socket(sockfd)),
          channel_(new Channel(loop, sockfd)),
          localAddr_(localAddr),
          peerAddr_(peerAddr)
{
    channel_->setWriteCallback(
            std::bind(&TcpConnection::handleWrite, this));
    channel_->setCloseCallback(
            std::bind(&TcpConnection::handleClose, this));
    channel_->setErrorCallback(
            std::bind(&TcpConnection::handleError, this));
//    channel_->setReadCallback(
//            std::bind(&TcpConnection::handleRead, this));
//    LOG_DEBUG << "TcpConnection::ctor[" <<  name_ << "] at " << this
//              << " fd=" << sockfd;
//    socket_->setKeepAlive(true);
}

void TcpConnection::handleRead(Timestamp receiveTime) {
    int savedErrno = 0l;
    ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);
    if (n > 0) {
        messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
    } else if (n == 0) {
        handleClose();
    } else {
        errno = savedErrno;
        handleError();
    }
}


void TcpConnection::handleClose() {
    loop_->assertInLoopThread();
    assert(state_ == kConnected || state_ ==kDisconnecting);
    setState(kDisconnected);
    channel_->disableAll();
    closeCallback_(shared_from_this());
}

void TcpConnection::handleError() {
    int err = sockets::getSocketError(channel_->fd());
}

void TcpConnection::connectDestroyed() {
    loop_->assertInLoopThread();
    if (state_ == kConnected) {
        setState(kDisconnected);
        channel_->disableAll();
        connectionCallback_(shared_from_this());
    }
    channel_->remove();
}

void TcpConnection::connectEstablished()
{
    loop_->assertInLoopThread();
    assert(state_ == kConnecting);
    setState(kConnected);
    channel_->tie(shared_from_this());
    channel_->enableReading();
    connectionCallback_(shared_from_this());
}

void TcpConnection::shutdown() {
    if (state_ == kConnected) {
        setState(kDisconnecting);
        loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
    }
}

void TcpConnection::shutdownInLoop() {
    loop_->assertInLoopThread();
    if (!channel_->isWriting()) {
        socket_->shutdownWrite();
    }
}

void TcpConnection::send(const std::string& message) {
    if (state_ == kConnected) {
        if (loop_->isInLoopThread()) {
            sendInLoop(message);
        } else {
            loop_->runInLoop(std::bind(&TcpConnection::sendInLoop, this, message));
        }
    }
}

void TcpConnection::sendInLoop(const std::string &message) {
    loop_->assertInLoopThread();
    ssize_t nwrote = 0;
    if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0) {
        nwrote = ::write(channel_->fd(), message.data(), message.size());
        if (nwrote >=0) {
            if (implicit_cast<size_t>(nwrote) < message.size()) {
                // write more data
            }
        } else {
            nwrote = 0;
            if (errno != EWOULDBLOCK) {
                // sendInLoop
            }
        }
    }
    assert(nwrote >=0);
    if (implicit_cast<size_t>(nwrote) < message.size()) {
        outputBuffer_.append(message.data() + nwrote, message.size() - nwrote);
        if (!channel_->isWriting()) {
            channel_->enableWriting();
        }
    }
}

void TcpConnection::handleWrite() {
    loop_->assertInLoopThread();
    if (channel_->isWriting()) {
        ssize_t n = ::write(channel_->fd(), outputBuffer_.peek(), outputBuffer_.readableBytes());
        if (n > 0) {
            outputBuffer_.retrieve(n);
            if (outputBuffer_.readableBytes() == 0) {
                channel_->disableWriting();
                if (state_ == kDisconnecting) {
                    shutdownInLoop();
                }
            } else {
                // going to write more data
            }
        } else {
            // tcp connection handle write
        }
    } else {
        // connection is down, no more writing
    }
}

void TcpConnection::setTcpNoDelay(bool on) {
    socket_->setTcpNoDelay(on);
}

void TcpConnection::forceClose() {
    // FIXME: use compare and swap
    if (state_ == kConnected || state_ == kDisconnecting) {
        setState(kDisconnecting);
        loop_->queueInLoop(std::bind(&TcpConnection::forceCloseInLoop, shared_from_this()));
    }
}

void TcpConnection::forceCloseInLoop() {
    loop_->assertInLoopThread();
    if (state_ == kConnected || state_ == kDisconnecting) {
        // as if we received 0 byte in handleRead();
        handleClose();
    }
}

TcpConnection::~TcpConnection()
{
//    LOG_DEBUG << "TcpConnection::dtor[" <<  name_ << "] at " << this
//              << " fd=" << channel_->fd()
//              << " state=" << stateToString();
    assert(state_ == kDisconnected);
}
