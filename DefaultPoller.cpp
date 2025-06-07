#include "Poller.hpp"
#include "EpollPoller.hpp"

Poller * Poller::newDefaultPoller(EventLoop *loop){
    if(::getenv("MUDUO_USE_POLL")){
        //暂时不实现。
        return nullptr;
    }else{
        return new EpollPoller(loop);
    }
}