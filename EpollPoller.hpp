#pragma once
#include "Poller.hpp"
/**
 * 主要对以下三个方法进行封装
 * epoll_create
 * epoll_ctl:add/mod/del
 * epoll_wait
 */


class EpollPoller : public Poller{
public:
    EpollPoller(EventLoop *);
    ~EpollPoller() override;

    //重写抽象基类方法。
    Timestamp poll(int timeoutMs, ChannelList *activeChannels) override;
    void updateChannel(Channel *channel) override;
    void removeChannel(Channel *channel) override;

private:
    //事件列表初始化长度
    static const int kInitEventListSize = 16;

    int epollfd_;

    //填写活跃的链接
    void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;

    //更新channel通道
    void update(int operation, Channel *channel);

    //定义一个事件列表
    using EventList = std::vector<epoll_event>;
    EventList events_;



};