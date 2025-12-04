#include "EventLoopThread.hpp"


EventLoopThread::EventLoopThread(const ThreadInitCallback &cb, const std::string &name)
    : loop_(nullptr),
      exiting_(false),
      thread_(std::bind(&EventLoopThread::threadFunc, this), name),
      mutex_(),
      cond_(),
      callBack_(cb)
{

}

EventLoopThread::~EventLoopThread(){
    this->exiting_ = true;
    if(this->loop_ != nullptr){
        this->loop_->quit();
        this->thread_.join();
    }
}

EventLoop* EventLoopThread::startLoop(){
    this->thread_.start();
    EventLoop *loop = nullptr;
    {
        std::unique_lock<std::mutex> lock(this->mutex_);
        while(this->loop_ == nullptr){
            this->cond_.wait(lock);
        }
        loop = this->loop_;
    }
    return loop;
}

// 线程中执行的函数
void EventLoopThread::threadFunc(){
    EventLoop loop; //创建一个独立的EventLoop对象，这是线程栈空间上的对象，不需要手动管理它的生命周期。
    if (this->callBack_)
    {
        //在新线程中运行回调函数
        this->callBack_(&loop);
    }
    {
        std::unique_lock<std::mutex> lock(this->mutex_);
        this->loop_ = &loop;
        this->cond_.notify_one();
    }
    loop.loop();
    std::unique_lock<std::mutex> lock(this->mutex_);
    this->loop_ = nullptr;
}