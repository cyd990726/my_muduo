#include "EventLoop.hpp"
#include <sys/eventfd.h>
#include <unistd.h>
#include <fcntl.h>
#include "Logger.hpp"
#include "Poller.hpp"
#include "Channel.hpp"
//防止一个线程里面创建多个EventLoop对象
__thread EventLoop *t_loopInThisThread = nullptr;

//定义默认的poller的超时时间
const int kPollTimeMs = 10000;

// 创建wakeupfd,用来notify唤醒subReactor处理新来的channel
int creatEventfd(){
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(evtfd < 0){
        //说明创建eventfd失败
        LOG_FATAL("Failed in eventfd");
    }
    return evtfd;
}

EventLoop::EventLoop()
    :looping_(false),
    quit_(false),
    callingPendingFunctors_(false),
    threadId_(CurrentThread::tid()),
    pollerReturnTime_(0),
    poller_(Poller::newDefaultPoller(this)),
    wakeupFd_(creatEventfd()),
    wakeupChannel_(new Channel(this, wakeupFd_)),
    currentActiveChannel_(nullptr)
{
    LOG_DEBUG("EventLoop created %p in thread %d", this, threadId_);
    if(t_loopInThisThread){
        //说明当前线程已经有一个loop了
        LOG_FATAL("Another EventLoop %p exists in this thread %d", t_loopInThisThread, threadId_);
    }else{
        t_loopInThisThread = this;
    }

    //设置wakeupChannel的读回调函数
    wakeupChannel_->setReadCallBack(std::bind(&EventLoop::handlerRead, this));
    //每一个eventloop都将监听wakechannel都EPOLLIN读事件
    wakeupChannel_->enableReading();
}


EventLoop::~EventLoop(){
    // 回收相应的资源
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    ::close(wakeupFd_);
    t_loopInThisThread = nullptr;
}


void EventLoop::loop(){
    this->looping_ = true;
    this->quit_ = false;
    LOG_INFO("EventLoop %p start looping", this);
    while(!this->quit_){
        activeChannels_.clear();
        this->pollerReturnTime_ = this->poller_->poll(kPollTimeMs, &activeChannels_);
        for(Channel *channel : activeChannels_){
            //Poller监听到哪些channel发生事件，将这些channel传递给EventLoop，EventLoop再调用channel的handleEvent方法处理事件
            this->currentActiveChannel_ = channel;
            this->currentActiveChannel_->handleEvent(this->pollerReturnTime_);
        }
        // 执行当前EventLoop事件循环需要处理的回调操作
        this->doPendingFunctors();
    }
}

void EventLoop::quit(){
    quit_ = true;
    // 在其他线程中调用的quit，需要唤醒loop所在的线程
    if(!isInLoopThread()){
        wakeup();
    }
}



void EventLoop::handlerRead(){
    uint64_t one = 1;
    ssize_t n = ::read(wakeupFd_, &one, sizeof(one));
    if(n != sizeof(one)){
        LOG_ERROR("EventLoop::handlerRead() reads %ld bytes instead of 8", n);
    }
}

void EventLoop::runInLoop(Functor cb){
    if(isInLoopThread()){
        cb();
    }
    else{
        queueInLoop(cb);
    }
}

//将cb放入队列中，唤醒loop所在的线程执行cb
void EventLoop::queueInLoop(Functor cb){
    {
        std::unique_lock<std::mutex> lock(mutex_);
        pendingFunctors_.emplace_back(cb);
    }
    // 唤醒相应的，需要执行上面回调操作的loop的线程了
    //callingPendingFunctors_表示当前loop正在执行回调操作，而我们又给他加了新的回调操作
    
    if(!isInLoopThread() || callingPendingFunctors_){
        wakeup();
    }
}

/// @brief 唤醒loop所在的线程
void EventLoop::wakeup(){
    uint64_t one = 1;
    ssize_t n = ::write(this->wakeupFd_, &one, sizeof(one));
    if(n != sizeof(one)){
        LOG_ERROR("EventLoop::wakeup() writes %ld bytes instead of 8", n);
    }
}

void EventLoop::updateChannel(Channel *channel){
    this->poller_->updateChannel(channel);
}
void EventLoop::removeChannel(Channel *channel){
    this->poller_->removeChannel(channel);
}
bool EventLoop::hasChannel(Channel *channel){
    return this->poller_->hasChannel(channel);
}

// 执行回调函数
void EventLoop::doPendingFunctors(){
    std::vector<Functor> functors;
    callingPendingFunctors_ = true; //正在执行回调

    {
        std::unique_lock<std::mutex> lock(mutex_);
        functors.swap(pendingFunctors_); // 交换后，减少临界区的时间
    }

    for(const Functor &functor : functors){
        functor();
    }
    callingPendingFunctors_ = false;


}