#pragma once
#include "noncopyable.hpp"
#include <vector>
#include <unordered_map>
#include "Timestamp.hpp"

class Channel;
class EventLoop;

//定义一个抽象基类（注意抽象基类必须要继承）
//可实现epoll、poll、select等多种Poller

class Poller : noncopyable{

public:
    using ChannelList = std::vector<Channel *>;

    Poller(EventLoop *loop);
    virtual ~Poller();

    //给所有IO复用保留统一接口
    virtual Timestamp poll(int timeoutMs, ChannelList *activeChannels) = 0;
    virtual void updateChannel(Channel *channel) = 0;
    virtual void removeChannel(Channel *channel) = 0;

    //判断channel是否在Poller中
    bool hasChannel(Channel *channel) const;

    // EventLoop可以通过该接口获取默认的IO复用的具体实现
    static Poller * newDefaultPoller(EventLoop *loop);
protected:
    //key:sockfd
    //value:Channel对象指针
    using ChannelMap = std::unordered_map<int, Channel *>;
    ChannelMap channels_;

private:
    EventLoop *ownerLoop_; //定义Poller所属的事件循环EventLoop
};