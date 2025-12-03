#include "Poller.hpp"
#include "EpollPoller.hpp"

// 为什么要单独拿出来一个文件呢，为了防止在基类里面包含派生类的头文件，
Poller * Poller::newDefaultPoller(EventLoop *loop){
    if(::getenv("MUDUO_USE_POLL")){
        //暂时不实现。
        return nullptr;
    }else{
        return new EpollPoller(loop);
    }
}