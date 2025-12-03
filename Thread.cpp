#include <semaphore.h>

#include "Thread.hpp"
#include "CurrentThread.hpp"

//静态成员变量在类外单独初始化
std::atomic_int Thread::numCreated_(0);

Thread::Thread(ThreadFunc func, const std::string &name)
    : started_(false),
      joined_(false),
      thread_(nullptr),
      tid_(0),
      func_(std::move(func)),
      name_(name)
{
    setDefaultName();

}
Thread::~Thread(){
    if(started_ && !joined_){
        //如果线程还没有被join，那么就detach它。
        thread_->detach();
    }
}

void Thread::start(){
    started_ = true;
    //用信号量来等待线程启动完成
    sem_t sem;
    sem_init(&sem, 0, 0);
    //创建新线程
    this->thread_ = std::make_shared<std::thread>(
        [&](){
            //在新线程中运行的函数
            this->tid_ = CurrentThread::tid();
            //线程启动完成，通知主线程
            sem_post(&sem);
            this->func_();
        }
    );
    //等待线程启动完成
    sem_wait(&sem);

}
void Thread::join(){
    this->joined_ = true;
    this->thread_->join();  
}

void Thread::setDefaultName(){
    int num = ++numCreated_;
    if(name_.empty()){
        char buf[32] = {0};
        snprintf(buf, sizeof(buf), "Thread%d", num);
        name_ = std::string(buf);
    }
}