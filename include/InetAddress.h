//
// Created by Yunan Chen on 2023/2/13.
//

#ifndef MUDUO_INETADDRESS_H
#define MUDUO_INETADDRESS_H
#include "noncopyable.h"
#include "SocketsOps.h"
#include <netinet/in.h>
#include <string>

class InetAddress {
public:
    explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false, bool ipv6 = false);

    InetAddress(std::string& ip, uint16_t port, bool ipv6 = false);

    explicit InetAddress(const struct sockaddr_in& addr)
            : addr_(addr)
    { }

    explicit InetAddress(const struct sockaddr_in6& addr)
            : addr6_(addr)
    { }

    sa_family_t family() const { return addr_.sin_family; }
    const struct sockaddr* getSockAddr() const { return sockets::sockaddr_cast(&addr6_); }
    void setSockAddrInet6(const struct sockaddr_in6& addr6) { addr6_ = addr6; }

    std::string toIpPort() const;

private:
    union {
        struct sockaddr_in addr_;
        struct sockaddr_in6 addr6_;
    };
};

#endif //MUDUO_INETADDRESS_H
