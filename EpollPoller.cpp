#include "EpollPoller.hpp"
#include <sys/epoll.h>
#include "Logger.hpp"
#include <unistd.h>

// channel未添加到poller中
const int kNew = -1;  // channel的成员index_ = -1
// channel已添加到poller中
const int kAdded = 1;
// channel从poller中删除
const int kDeleted = 2;

EpollPoller::EpollPoller(EventLoop *loop)
    :Poller(loop), 
    epollfd_(::epoll_create1(EPOLL_CLOEXEC)), //EPOLL_CLOEXEC表示当执行exec系列的函数执行新程序时，自动关闭文件描述符，能避免资源泄露
    events_(kInitEventListSize) {
    
    if(epollfd_ < 0){
        LOG_FATAL("epoll_create error:%d\n", errno);
    }

}

EpollPoller::~EpollPoller(){
    ::close(epollfd_);
}

// //重写抽象基类方法。
// Timestamp poll(int timeoutMs, ChannelList *activeChannels) override;
// void updateChannel(Channel *channel) override;
// void removeChannel(Channel *channel) override;