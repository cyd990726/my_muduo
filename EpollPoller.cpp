#include "EpollPoller.hpp"
#include <sys/epoll.h>
#include "Logger.hpp"
#include <unistd.h>
#include "Channel.hpp"
#include <string.h>
// 一下是channel的三个状态码，用于标识Channel的状态
// channel表示未添加到poller中
const int kNew = -1;  // channel的成员index_ = -1
// channel已添加到poller中
const int kAdded = 1;
// channel从poller中删除
const int kDeleted = 2;

EpollPoller::EpollPoller(EventLoop *loop)
    :Poller(loop), 
    epollfd_(::epoll_create1(EPOLL_CLOEXEC)), //EPOLL_CLOEXEC表示当执行exec系列的函数执行新程序时，自动关闭文件描述符，能避免资源泄露
    events_(kInitEventListSize) 
{
    
    if(epollfd_ < 0){
        LOG_FATAL("epoll_create error:%d\n", errno);
    }

}

EpollPoller::~EpollPoller(){
    ::close(epollfd_);
}

// //重写抽象基类方法。
Timestamp EpollPoller::poll(int timeoutMs, ChannelList *activeChannels){
    LOG_DEBUG("func=%s => fd total count:%lu \n", __FUNCTION__, channels_.size());

}

// updateChannel和removeChannel对应epoll_ctl
// channel的update和remove触发EventLoop的update和remove
// 再调用Poller的update和remove
void EpollPoller::updateChannel(Channel *channel){
    const int index = channel->index();
    if(index == kNew || index == kAdded){
        if(index == kNew){
            //说明是新节点，未添加到Poller
            this->channels_.insert(std::make_pair(channel->fd(), channel));  
        }
        //更新channel的状态
        channel->set_index(kNew);
        update(EPOLL_CTL_ADD, channel);
    }else{
        //channel已经再Poller上注册过了
        int fd = channel->fd();
        if(channel->isNoneEvent()){
            //如果channel没有事件了，说明是从Poller中删除
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        }else{
            //更新事件
            update(EPOLL_CTL_MOD, channel);  
        }
    }
}


void EpollPoller::removeChannel(Channel *channel){
    //先从channelMap中删除对应的元素
    int fd = channel->fd();
    channels_.erase(fd);

    LOG_DEBUG("func=%s => fd=%d\n", __FUNCTION__, fd);
    
    //删除
    int index = channel->index();
    if (index == kAdded)
    {
        update(EPOLL_CTL_DEL, channel);
    }
    //设置channel的状态为kNew，因为它已经完全从Poller中删除了。
    channel->set_index(kNew);
}

//更新
void EpollPoller::update(int operation, Channel *channel){
    //定义一个epoll_event
    epoll_event event;
    //清空初始化
    bzero(&event, sizeof(event));

    int fd = channel->fd();

    //设置监听事件
    event.events = channel->events();
    event.data.fd = fd;
    event.data.ptr = channel;

    //（添加、修改、删除）监控的文件描述及其事件
    if(::epoll_ctl(this->epollfd_, operation, fd, &event) < 0){
        //上树失败
        if(operation == EPOLL_CTL_DEL){
            //如果是删除操作，可能是因为fd已经被关闭了
            LOG_DEBUG("epoll_ctl op=%d fd=%d error:%s\n", operation, fd, strerror(errno));
        }else{
            //打印错误信息
            LOG_FATAL("epoll_ctl op=%d fd=%d error:%s\n", operation, fd, strerror(errno));  
        }
        
    }
}

//将epoll_wait接受到的活跃的连接，填写在channel中
void EpollPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const
{
    for (int i=0; i < numEvents; ++i)
    {
        Channel *channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel); // EventLoop就拿到了它的poller给它返回的所有发生事件的channel列表了
    }
}

Timestamp EpollPoller::poll(int timeoutMs, ChannelList *activeChannels){
    LOG_DEBUG("func=%s => fd total count:%lu \n", __FUNCTION__, channels_.size());
    
}