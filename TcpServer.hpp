#pragma once

#include "noncopyable.hpp"
#include "Acceptor.hpp"
#include "EventLoopThreadPool.hpp"
#include "Callbacks.hpp"
#include <unordered_map>


class TcpServer {

public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;

    enum Option{
        kNoReusePort,
        kReusePort,
    };

    TcpServer(EventLoop *loop,
            const InetAddress &listenAddr,
            const std::string &nameArg,
            Option option = kNoReusePort);
    ~TcpServer();
    
    void setThreadInitCallback(const ThreadInitCallback &cb){
        this->threadInitCallback_ = cb;
    }
    void setConnectionCallback(const ConnectionCallback &cb){
        this->connectionCallback_ = cb;
    }   
    
    void setMessageCallback(const MessageCallback &cb){
        this->messageCallback_ = cb;
    }

    void setWriteCompleteCallback(const WriteCompleteCallback &cb){
        this->writeCompleteCallback_ = cb;
    }

    void setThreadNum(int numThreads){
        this->threadPool_->setThreadNum(numThreads);
    }

    void start();//开启服务器监听

private:
    void newConnection(int sockfd, const InetAddress &peerAddr);
    void removeConnection(const TcpConnectionPtr &conn);
    void removeConnectionInLoop(const TcpConnectionPtr &conn);

    using ConnectionMap = std::unordered_map<std::string, TcpConnectionPtr>;

    EventLoop *loop_;
    const std::string ipPort_;
    const std::string name_;
    std::unique_ptr<Acceptor> acceptor_;
    std::shared_ptr<EventLoopThreadPool> threadPool_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;

    ThreadInitCallback threadInitCallback_;
    std::atomic_int started_;
    int nextConnId_;
    ConnectionMap connections_;



};