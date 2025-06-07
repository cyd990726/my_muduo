#include "InetAddress.hpp"
#include "string.h"

InetAddress::InetAddress(uint16_t port, std::string ip){
    bzero(&this->addr_, sizeof(this->addr_));
    this->addr_.sin_family = AF_INET;//使用IPv4协议
    //记得转换成网络字节序
    this->addr_.sin_port = htons(port);
    //将点分十进制的IPv4字符串哈尊换成网络字节序的32位二进制IP地址
    //前面加::表示使用全局作用于下的函数，而不是本地命名空间的函数
    this->addr_.sin_addr.s_addr = ::inet_addr(ip.c_str());
}

std::string InetAddress::toIp() const{
    char buf[64] = {0};
    //网络地址s_addr转换成本地字符串
    ::inet_ntop(AF_INET, &this->addr_.sin_addr.s_addr, buf, sizeof(buf));
    return std::string(buf);
}

std::string InetAddress::toIpPort() const{
    char buf[64] = {0};
    ::inet_ntop(AF_INET, &this->addr_.sin_addr.s_addr, buf, sizeof(buf));
    //求出地址ip地址写入的长度
    size_t end = strlen(buf);
    //网络字节序转换成本地字节序
    uint16_t port = ntohs(this->addr_.sin_port);
    //继续写入buf
    snprintf(buf+end, sizeof(buf)-end, ":%u", port);
    //返回
    return std::string(buf);
}
uint16_t InetAddress::toPort() const{
    //直接返回端口，记得转换成本地字节序
    return ntohs(this->addr_.sin_port);
}
