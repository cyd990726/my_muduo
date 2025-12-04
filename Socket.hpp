#pragma once

#include "noncopyable.hpp"
#include "InetAddress.hpp"
#include <string.h>


class Socket{
public:
    explicit Socket(int sockfd)
        :sockfd_(sockfd){}
    ~Socket();
    int fd() const { return sockfd_; }

    void bindAddress(const InetAddress &localaddr);
    void listen();
    int accept(InetAddress *peeraddr);

    void shutdownWrite();
    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);

private:
    int sockfd_;
};