#include "Channel.hpp"
#include <sys/epoll.h>
#include "EventLoop.hpp"

//初始化类内的静态变量，用const我猜是怕被修改
const int Channel::kNoneEvent = 0;
//注意：EPOLLIN是普通读事件，而EPOLLPRI是有紧急数据到达
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop), fd_(fd), events_(kNoneEvent), revents_(kNoneEvent), index_(-1), tied_(false)
{   
}
//muduo源码中析构函数立里面全是断言，
//用于保证被当前线程所在的loop析构，没有逻辑代码，所以这里不写
Channel::~Channel()
{
}

//绑定
void Channel::tie(const std::shared_ptr<void> &obj){
    this->tie_= obj;
    tied_ = true;
}

/**
 * 当改变channel所表示的fd的events事件后，
 * update负责在poller里面更改fd相应事件epoll_ctl
 */
void Channel::update(){
    // 通过channel所属的EventLoop，调用poller的相应方法，注册fd的events事件
    this->loop_->updateChannel(this);
}

//在channel所属的EventLoop中，把当前的channel删除掉，最终是调用到Poller的removeChannel方法
void Channel::remove(){
    this->loop_->removeChannel(this);
}

void Channel::handleEvent(Timestamp receiveTime){
    if(this->tied_){
        //如果是在监控外部对象
        std::shared_ptr<void> guard =  this->tie_.lock();
        if(guard){
            //如果外部对象的引用计数为0，即析构了。
            handleEventWithGuard(receiveTime);
        }
    }else{
        //如果没有监控外部对象
       handleEventWithGuard(receiveTime); 
    }
}

//根据接受到的事件执行相应的回调操作
void Channel::handleEventWithGuard(Timestamp receiveTime){
    //如果是关闭事件且没有读事件
    if((this->revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)){
        //关掉channel
        if(this->closeCallback_){
            //如果设置了相应的回调函数，就执行相应的回调函数
            this->closeCallback_();
        }
    }

    //处理错误事件
    if(this->revents_ & EPOLLERR){
        if(this->errorCallback_){
            this->errorCallback_();
        }
    }

    //处理读事件
    if(this->revents_ & (EPOLLIN | EPOLLPRI)){
        if(this->readCallback_){
            this->readCallback_(receiveTime);
        }
    }

    //处理写事件
    if(this->revents_ & EPOLLOUT){
        if(this->writeCallback_){
            this->writeCallback_();
        }
    }
}
