#pragma once


#include <functional>
#include <thread>
#include <memory>
#include<unistd.h>
#include <sys/types.h>
#include <string>
#include <atomic>


#include "noncopyable.hpp"


class Thread : noncopyable {
public:
    using ThreadFunc = std::function<void()>;
    Thread() = delete; //阻止默认构造
    explicit Thread(ThreadFunc func, const std::string &name);
    ~Thread();

    void start();
    void join();
    pid_t tid() const { return tid_; }
    const std::string &name() const { return name_; }
    static int numCreated(){ return numCreated_.load(); }

private:
    void setDefaultName();

    bool started_;
    bool joined_;
    std::shared_ptr<std::thread> thread_;
    pid_t tid_;
    ThreadFunc func_;
    std::string name_;
    static std::atomic_int numCreated_; //记录创建的线程数量

};