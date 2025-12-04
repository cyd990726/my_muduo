#pragma once

#include <functional>

#include "noncopyable.hpp"
#include "Socket.hpp"
#include "EventLoop.hpp"
#include "Channel.hpp"

class Acceptor: noncopyable{

public:
    using NewConnectionCallback = std::function<void(int sockfd, const InetAddress &)> ;
    Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback &cb){
        this->newConnectionCallback_ = std::move(cb);
    }

    bool listening() const { return listening_; }
    void listen();

private:    
    void handleRead(); //有新连接到来时的回调函数
    
    EventLoop *loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listening_;

};