#include "EpollPoller.hpp"
#include <sys/epoll.h>
#include "Logger.hpp"
#include <unistd.h>
#include "Channel.hpp"
#include <string.h>
// 一下是channel的三个状态码，用于标识Channel的状态

enum CHANNEL_STATE_CODE{
    kNew = -1,
    kAdded = 1,
    kDeleted = 2,
};

EpollPoller::EpollPoller(EventLoop *loop)
    :Poller(loop), 
    epollfd_(::epoll_create1(EPOLL_CLOEXEC)), //EPOLL_CLOEXEC表示当执行exec系列的函数执行新程序时，自动关闭文件描述符，能避免资源泄露
    events_(kInitEventListSize) 
{
    
    if(epollfd_ < 0){
        //说明创建epollfd失败
        LOG_FATAL("epoll_create error:%d\n", errno);
        // 下面这一行可以省略，在LOG_FATAL中已经exit了
        // exit(EXIT_FAILURE);
    }

}

EpollPoller::~EpollPoller(){
    ::close(epollfd_);
}

// //重写抽象基类方法。
Timestamp EpollPoller::poll(int timeoutMs, ChannelList *activeChannels){
    LOG_DEBUG("func=%s => fd total count:%lu \n", __FUNCTION__, channels_.size());
    int numEvents = epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutMs);
    int saveerrno = errno; //先用一个局部变量把errno存起来，因为多个线程都会用到errno， 防止其他线程修改了errno
    Timestamp now = Timestamp::now();//记录具体发生事件的时间点
    if(numEvents > 0){
        //说明有事件发生
        LOG_DEBUG("%d events happened\n", numEvents);
        // 将发生的事件填充到activeChannels中
        fillActiveChannels(numEvents, activeChannels);
        if(numEvents == events_.size()){
            //说明事件队列满了，需要扩容
            events_.resize(events_.size() * 2);
        }
    }else if(numEvents == 0){
        //说明超时了
        LOG_DEBUG("func=%s => timeout\n", __FUNCTION__);
    }else{
        if(saveerrno != EINTR){
            //说明epoll_wait出错了
            errno = saveerrno; //恢复errno
            LOG_ERROR("EpollPoller::poll() err!");
        }
    }
    return now;
}

// updateChannel和removeChannel对应epoll_ctl
// channel的update和remove触发EventLoop的update和remove
// 再调用Poller的update和remove
void EpollPoller::updateChannel(Channel *channel){
    const int index = channel->index();
    if(index == CHANNEL_STATE_CODE::kNew || index == CHANNEL_STATE_CODE::kAdded){
        if(index == CHANNEL_STATE_CODE::kNew){
            //说明是新节点，未添加到Poller
            this->channels_.insert(std::make_pair(channel->fd(), channel));  
        }
        //更新channel的状态
        channel->set_index(CHANNEL_STATE_CODE::kAdded);
        update(EPOLL_CTL_ADD, channel);
    }else{
        //channel已经再Poller上注册过了
        int fd = channel->fd();
        if(channel->isNoneEvent()){
            //如果channel没有事件了，说明是从Poller中删除
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(CHANNEL_STATE_CODE::kDeleted);
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
    if (index == CHANNEL_STATE_CODE::kAdded)
    {
        update(EPOLL_CTL_DEL, channel);
    }
    //设置channel的状态为kNew，因为它已经完全从Poller中删除了。
    channel->set_index(CHANNEL_STATE_CODE::kNew);
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
