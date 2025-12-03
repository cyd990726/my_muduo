#pragma once

#include <mutex>
#include <condition_variable>

#include "Thread.hpp"
#include "noncopyable.hpp"
#include "EventLoop.hpp"


class EventLoopThread:noncopyable{
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;
    EventLoopThread(const ThreadInitCallback &cb = ThreadInitCallback(), const std::string &name = std::string());
    ~EventLoopThread();

    EventLoop* startLoop();

private:
    EventLoop *loop_;
    bool exiting_;
    Thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
    ThreadInitCallback callBack_;
    void threadFunc();

};