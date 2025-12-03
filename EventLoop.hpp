#pragma once

#include "noncopyable.hpp"
#include <functional>
#include <vector>
#include <atomic>
#include "Timestamp.hpp"
#include <memory>
#include <mutex>
#include "CurrentThread.hpp"



//前置声明
class Channel;
class Poller;

//事件循环类，主要包含两个大模块：Channel和Poller（epoll的抽象）
class EventLoop: noncopyable {

public:
    
    using Functor =  std::function<void()>;
    EventLoop();
    ~EventLoop();

    void loop(); //事件循环函数
    void quit(); //退出事件循环
    Timestamp pollerReturnTime() const { return this->pollerReturnTime_; }
    void runInLoop(Functor cb); //在当前loop中执行回调操作
    void queueInLoop(Functor cb); //把回调操作放入队列

    void wakeup(); //唤醒loop所在的线程

    void updateChannel(Channel *channel); //更新channel
    void removeChannel(Channel *channel); //删除channel
    bool hasChannel(Channel *channel); //判断是否有该channel
    bool isInLoopThread() const { return this->threadId_ == CurrentThread::tid(); } //判断当前loop是否在自己的线程中运行

private:
    void handlerRead(); //唤醒loop所在线程的回调函数
    void doPendingFunctors(); //执行回调操作

    // 定义一个channel的列表
    using ChannelList = std::vector<Channel*>;

    std::atomic_bool looping_; //原子变量，标识是否正在循环
    std::atomic_bool quit_; //原子变量, 标识退出loop循环
    std::atomic_bool callingPendingFunctors_; //原子变量，标识当前loop是否有需要执行的回调操作

    const pid_t threadId_; // 记录当前loop所在线程的id
    Timestamp pollerReturnTime_; // Poller上一次返回发生事件的时间点
    std::unique_ptr<Poller> poller_; // poller指针

    int wakeupFd_; // 当mainLoop获取到一个新的channel时，通过轮询算法选择一个subLoop，通过该成员唤醒subLoop处理channel
    std::unique_ptr<Channel> wakeupChannel_; // 用于唤醒loop所在线程的channel

    ChannelList activeChannels_; // 记录当前loop正在处理的channel列表
    Channel *currentActiveChannel_; // 记录当前正在处理的channel

    std::vector<Functor> pendingFunctors_; // 存储loop需要执行的回调操作列表
    std::mutex mutex_; // 保护pendingFunctors_的互斥锁
};