#pragma once

#include <functional>
#include <vector>
#include <string>
#include <memory>

#include "noncopyable.hpp"
#include "EventLoop.hpp"
#include "EventLoopThread.hpp"

class EventLoopThreadPool: noncopyable{
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;
    EventLoopThreadPool(EventLoop *baseLoop, const std::string &nameArg);
    ~EventLoopThreadPool();

    void setThreadNum(int numThreads){ this->numThreads_ = numThreads; }; //设置线程池中的线程数量
    void start(const ThreadInitCallback &cb = ThreadInitCallback()); //启动线程池
    EventLoop* getNextLoop(); //获取下一个subLoop，轮询分配
    std::vector<EventLoop*> getAllLoops(); //获取所有的subLoop
    bool started() const { return this->started_; } //判断线程池是否已经启动
    const std::string& name() const { return this->name_; } //获取线程池名称


private:
    EventLoop* baseLoop_; //mainLoop，用户创建的
    std::string name_; //线程池名称
    bool started_; //线程池是否已经启动
    int numThreads_; //线程池中的线程数量
    int next_; //下一个被分配的subLoop的序号
    std::vector<std::unique_ptr<EventLoopThread>> threads_; //存放EventLoopThread的智能指针
    std::vector<EventLoop*> loops_; //存放subLoop的指针


};