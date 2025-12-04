#include "TcpServer.hpp"    
#include "Logger.hpp"



//检查EventLoop是不是nullptr，是的话就终止
EventLoop * CheckLoopNotNull(EventLoop *loop){
    if(loop == nullptr){
        LOG_FATAL("mainLoop is null!");
    }
    return loop;
}

TcpServer::TcpServer(EventLoop *loop, 
    const InetAddress &listenAddr, 
    const std::string &nameArg, 
    Option option)
        :loop_(CheckLoopNotNull(loop)),
        ipPort_(listenAddr.toIpPort()),
        name_(nameArg),
        acceptor_(new Acceptor(loop, listenAddr, option==kReusePort)),
        threadPool_(new EventLoopThreadPool(loop, nameArg)),
        connectionCallback_(),
        messageCallback_(),
        nextConnId_(1),
        started_(0)

{
    //有新连接来执行的回调函数
    acceptor_->setNewConnectionCallback(
        std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
    

}
TcpServer::~TcpServer(){

}

void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr){

   
}
void TcpServer::removeConnection(const TcpConnectionPtr &conn){

}
void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn){

}

void TcpServer::start(){
    if(started_++ == 0){
        //开启线程池
        this->threadPool_->start(threadInitCallback_);
        //启动loop开始监听
        this->loop_->runInLoop(
            std::bind(&Acceptor::listen, acceptor_.get())
        );
    }
}