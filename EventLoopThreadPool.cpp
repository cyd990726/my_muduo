#include "EventLoopThreadPool.hpp"


EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop, const std::string &nameArg)
    : baseLoop_(baseLoop),
      name_(nameArg),
      started_(false),
      numThreads_(0),
      next_(0)
{

}

EventLoopThreadPool::~EventLoopThreadPool(){

}

void EventLoopThreadPool::start(const ThreadInitCallback &cb){

    this->started_ = true;
    for(int i = 0; i<numThreads_; i++){
        //创建线程对象，线程命名为线程池名+线程序号
        char buf[name_.size() + 32];
        snprintf(buf, sizeof(buf), "%s%d", name_.c_str(), i);
        EventLoopThread *t = new EventLoopThread(cb, buf);
        this->threads_.push_back(std::unique_ptr<EventLoopThread>(t));
        //启动loop，并把loop指针返回，保存在线程池对象中
        this->loops_.push_back(t->startLoop());
    }
    if(numThreads_ == 0 && cb){
        //如果没有subLoop线程池，那么就在baseLoop中运行回调函数
        cb(this->baseLoop_);
    }



}
EventLoop* EventLoopThreadPool::getNextLoop(){
    EventLoop *loop = this->baseLoop_;
    if(!this->loops_.empty()){
        //如果有subLoop，那么就采用轮询的方式分配subLoop
        loop = this->loops_[this->next_];
        this->next_++;
        if(static_cast<size_t>(this->next_) >= this->loops_.size()){
            this->next_ = 0;
        }
    }
    return loop;
}
std::vector<EventLoop*> EventLoopThreadPool::getAllLoops(){
    if(this->loops_.empty()){
        return std::vector<EventLoop*>{this->baseLoop_};
    }else{
        return this->loops_;
    }
}
