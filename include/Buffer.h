//
// Created by Yunan Chen on 2023/2/20.
//

#ifndef MUDUO_BUFFER_H
#define MUDUO_BUFFER_H
#include <vector>
#include <string>
#include <cassert>
#include "../include/SocketsOps.h"
#include <sys/uio.h>

class Buffer {
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;
    explicit Buffer(size_t initialSize = kInitialSize) : buffer_(kCheapPrepend + initialSize),
    readerIndex_(kCheapPrepend), writerIndex_(kCheapPrepend) {
        assert(readableBytes() == 0);
        assert(writableBytes() == initialSize);
        assert(prependableBytes() == kCheapPrepend);
    }

    size_t readableBytes() const {
        return writerIndex_ - readerIndex_;
    }
    size_t writableBytes() const {
        return buffer_.size() - writerIndex_;
    }
    size_t prependableBytes() const {
        return readerIndex_;
    }

    void retrieveAll() {
        readerIndex_ = kCheapPrepend;
        writerIndex_ = kCheapPrepend;
    }
    void retrieve(size_t len) {
        assert(len <= readableBytes());
        if (len < readableBytes()) {
            readerIndex_ += len;
        }
        else {
            retrieveAll();
        }
    }

    const char* peek() const {
        return begin() + readerIndex_;
    }

    std::string retrieveAllAsString() {
        return retrieveAsString(readableBytes());
    }

    std::string retrieveAsString(size_t len) {
        assert(len <= readableBytes());
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

    void ensureWritableBytes(size_t len) {
        if (writableBytes() < len) {
            makeSpace(len);
        }
        assert(writableBytes() >= len);
    }

    void append(const char* /*restrict*/ data, size_t len) {
        ensureWritableBytes(len);
        std::copy(data, data+len, beginWrite());
        hasWritten(len);
    }

    void append(const void* /*restrict*/ data, size_t len) {
        append(static_cast<const char*>(data), len);
    }

    void prepend(const void* /*restrict*/ data, size_t len) {
        assert(len <= prependableBytes());
        readerIndex_ -= len;
        const char* d = static_cast<const char*>(data);
        std::copy(d, d + len, begin()+readerIndex_);
    }

    void swap(Buffer& rhs) {
        buffer_.swap(rhs.buffer_);
        std::swap(readerIndex_, rhs.readerIndex_);
        std::swap(writerIndex_, rhs.writerIndex_);
    }

//    ssize_t readFd(int fd, int* savedErrno);
    ssize_t readFd(int fd, int* savedErrno)
    {
        // saved an ioctl()/FIONREAD call to tell how much to read
        char extrabuf[65536];
        struct iovec vec[2];
        const size_t writable = writableBytes();
        vec[0].iov_base = begin() + writerIndex_;
        vec[0].iov_len = writable;
        vec[1].iov_base = extrabuf;
        vec[1].iov_len = sizeof extrabuf;
        // when there is enough space in this buffer, don't read into extrabuf.
        // when extrabuf is used, we read 128k-1 bytes at most.
        const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
        const ssize_t n = sockets::readv(fd, vec, iovcnt);
        if (n < 0)
        {
            *savedErrno = errno;
        }
        else if (implicit_cast<size_t>(n) <= writable)
        {
            writerIndex_ += n;
        }
        else
        {
            writerIndex_ = buffer_.size();
            append(extrabuf, n - writable);
        }
        // if (n == writable + sizeof extrabuf)
        // {
        //   goto line_30;
        // }
        return n;
    }
private:
    void hasWritten(size_t len) {
        assert(len <= writableBytes());
        writerIndex_ += len;
    }

    char* beginWrite()
    { return begin() + writerIndex_; }

    char* begin()
    { return &*buffer_.begin(); }

    const char* begin() const
    { return &*buffer_.begin(); }

    void makeSpace(size_t len) {
        if (writableBytes() + prependableBytes() < len + kCheapPrepend) {
            // FIXME: move readable data
            buffer_.resize(writerIndex_+len);
        }
        else {
            // move readable data to the front, make space inside buffer
            assert(kCheapPrepend < readerIndex_);
            size_t readable = readableBytes();
            std::copy(begin()+readerIndex_,
                      begin()+writerIndex_,
                      begin()+kCheapPrepend);
            readerIndex_ = kCheapPrepend;
            writerIndex_ = readerIndex_ + readable;
            assert(readable == readableBytes());
        }
    }

    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;

};

#endif //MUDUO_BUFFER_H
