#pragma once
#include "noncopyable.hpp"
#include <functional>
#include "Timestamp.hpp"
#include <memory>
//Channel可以理解为通道，封装了其感兴趣的event，如EPOLLIN, EPOLLOUT等
//还绑定了poller返回的具体事件

//前向声明，避免循环引用，减少编译时候没必要的头文件的包含，加快编译速度
//在头文件中使用前置声明而不是包含完整定义，减少暴露内部实现
class EventLoop; 

class Channel : noncopyable {
public:
    //声明回调函数类
    using EventCallBack = std::function<void()>;
    using ReadEventCallback = std::function<void(Timestamp)>;

    Channel(EventLoop *loop, int fd);
    ~Channel();

    // fd得到poller通知以后，处理事件的函数
    void handleEvent(Timestamp receiveTime);

    //设置回调函数
    void setReadCallBack(ReadEventCallback cb) {
        //通过move减少一次资源的拷贝
        this->readCallback_ = std::move(cb);
    }
    void setWriteCallBack(EventCallBack cb) {
        this->writeCallback_ = std::move(cb);
    }
    void setErrorCallBack(EventCallBack cb) {
        this->errorCallback_ = std::move(cb);
    }
    void setCloseCallBack(EventCallBack cb){
        this->closeCallback_ = std::move(cb);
    }

    //防止当channel被手动remove掉，channel还在执行回调操作
    void tie(const std::shared_ptr<void> &);

    int fd  () const { return this->fd_; } //获取fd
    int events() const { return this->events_; } //获取感兴趣的事件
    void set_revents(int revt) { this->revents_ = revt; } //设置poller返回的事件

    //设置fd相应的事件状态
    void enableReading() { this->events_ |= kReadEvent; update(); } //启用读事件
    void disableReading() { this->events_ &= ~kReadEvent; update(); } //禁用读事件
    void enableWriting() { this->events_ |= kWriteEvent; update(); } //启用写事件
    void disableWriting() { this->events_ &= ~kWriteEvent; update(); } //禁用写事件
    void disableAll() { this->events_ = kNoneEvent; update(); } //禁用所有事件

    //判断fd当前的事件状态
    bool isNoneEvent() const { return this->events_ == kNoneEvent; }
    bool isReading() const { return this->events_ & kReadEvent; }
    bool isWriting() const { return this->events_ & kWriteEvent; }

    int index() const { return this->index_; }
    void set_index(int index) { this->index_ = index; }

    EventLoop * ownerLoop() const { return this->loop_; }
    void remove(); //从poller中删除channel

private:
    void update(); //更新poller中的事件
    void handleEventWithGuard(Timestamp receiveTime);

    static const int kNoneEvent; //没有事件
    static const int kReadEvent; //读事件
    static const int kWriteEvent; //写事件

    EventLoop *loop_; // 事件循环
    int fd_; // fd, Poller监听的对象。
    int events_; // 注册fd感兴趣的事件
    int revents_; // poller返回的具体发生的事件
    int index_;

    std::weak_ptr<void> tie_; 
    bool tied_; //是否绑定了一个对象，防止在对象被销毁后，回调函数仍然在执行

    //因为channel里面能够获知fd最终发生的具体的事件的revents，
    //所以它负责调用具体的事件的回调操作
    ReadEventCallback readCallback_; //读事件回调函数
    EventCallBack writeCallback_; //写事件回调函数
    EventCallBack errorCallback_; //错误事件回调函数
    EventCallBack closeCallback_; //关闭事件回调函数
};