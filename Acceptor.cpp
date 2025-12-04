#include "Acceptor.hpp"
#include "Logger.hpp"

// 创建一个非阻塞的socket，失败则终止程序
static int createNonblockingOrDie(){
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if(sockfd < 0){
        LOG_FATAL("sockets::createNonblockingOrDie");
    }
    return sockfd;
}

Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport)
    :loop_(loop),
    acceptSocket_(createNonblockingOrDie()),
    acceptChannel_(loop, acceptSocket_.fd()),
    listening_(false)
{
    //设置acceptSocket_的相关属性
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.setReusePort(reuseport);
    //绑定地址
    acceptSocket_.bindAddress(listenAddr);

    //设置acceptChannel_的读回调函数
    acceptChannel_.setReadCallBack(std::bind(&Acceptor::handleRead, this));

}
Acceptor::~Acceptor(){

}


void Acceptor::listen(){

}
   
//listenfd有新连接到来时的回调函数
void Acceptor::handleRead(){
    InetAddress peeraddr;
    int connfd = acceptSocket_.accept(&peeraddr);
    if(connfd >= 0){
        if(this->newConnectionCallback_){
            //有新连接到来，调用回调函数（轮询找到subLoop唤醒，分发当前的新客户端的Channel）
            this->newConnectionCallback_(connfd, peeraddr);
        }else{
            //没有设置回调函数，关闭连接
            ::close(connfd);
        }
    }else{
        // 说明文件描述符资源用完了
        LOG_ERROR("in Acceptor::handleRead");
        if(errno == EMFILE){
            LOG_ERROR("Acceptor::handleRead - out of file descriptor");
        }
    }
}