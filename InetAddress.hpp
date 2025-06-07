#pragma once
#include <arpa/inet.h>
#include <string>

class InetAddress {

public:
    explicit InetAddress(uint16_t port = 0, std::string ip = "127.0.0.1");
    explicit InetAddress(const sockaddr_in &addr):addr_(addr){}

    //返回字符串IP和/或端口的字符串形式
    std::string toIp() const;
    std::string toIpPort() const;
    uint16_t toPort() const;

    //返回const指针是为了避免被从外部修改内部变量
    const sockaddr_in* getSockAddr() const {return &addr_;}
    void setSockAddr(const sockaddr_in &addr) { addr_ = addr; }
private:
    sockaddr_in addr_;
};