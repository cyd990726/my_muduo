#pragma once

class Channel;

//事件循环类，主要包含两个大模块：Channel和Poller（epoll的抽象）
class EventLoop {

public:
    void updateChannel(Channel *);
    void removeChannel(Channel *);


private:


};