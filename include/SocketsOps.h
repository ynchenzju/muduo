//
// Created by Yunan Chen on 2023/2/13.
//

#ifndef MUDUO_SOCKETSOPS_H
#define MUDUO_SOCKETSOPS_H

#include <netinet/in.h>
#include <sys/socket.h>

namespace sockets {
    void close(int sockfd);
    int  accept(int sockfd, struct sockaddr_in6* addr);
    int  connect(int sockfd, const struct sockaddr* addr);

    const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr);
    const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr);
    const struct sockaddr_in* sockaddr_in_cast(const struct sockaddr* addr);
    const struct sockaddr_in6* sockaddr_in6_cast(const struct sockaddr* addr);

    void toIpPort(char* buf, size_t size, const struct sockaddr* addr);
    void toIp(char* buf, size_t size, const struct sockaddr* addr);
    struct sockaddr* sockaddr_cast(struct sockaddr_in6* addr);
    int createNonblockingOrDie(sa_family_t family);
    void listenOrDie(int sockfd);
    void bindOrDie(int sockfd, const struct sockaddr* addr);
    void shutdownWrite(int sockfd);

    inline uint16_t hostToNetwork16(uint16_t host16) {
        return htobe16(host16);
    }

    inline uint32_t hostToNetwork32(uint32_t host32) {
        return htobe32(host32);
    }

    void fromIpPort(const char* ip, uint16_t port, struct sockaddr_in6* addr);
    void fromIpPort(const char* ip, uint16_t port, struct sockaddr_in* addr);
    struct sockaddr_in6 getLocalAddr(int sockfd);
    int getSocketError(int sockfd);
    ssize_t readv(int sockfd, const struct iovec *iov, int iovcnt);
    struct sockaddr_in6 getPeerAddr(int sockfd);
    bool isSelfConnect(int sockfd);
}

template<typename To, typename From>
inline To implicit_cast(From const &f)
{
    return f;
}
#endif //MUDUO_SOCKETSOPS_H
