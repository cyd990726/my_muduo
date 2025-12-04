#include <thread>
#include <sys/syscall.h>
#include <iostream>
#include <unistd.h>
#include <mutex>
__thread int t_cachedTid;
std::mutex mutex_;
void functionCallback(){
    int tid;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        tid = static_cast<int>(::syscall(SYS_gettid)); // 获取线程ID
        t_cachedTid = tid;
        pid_t pid = getpid(); // 获取进程ID
        std::cout << "进程ID："<< pid <<", 线程ID：" << t_cachedTid << std::endl;
    }
    //休眠两秒
    std::this_thread::sleep_for(std::chrono::seconds(2));
    if(tid == t_cachedTid){
        std::cout << "一致" << std::endl;
    }else{ 
        std::cout << "tid:" <<tid<< ", t_cachedTid:" << t_cachedTid << "不一致" << std::endl;
    }
    
}

int main(){
    std::thread t1(functionCallback);
    std::thread t2(functionCallback);
    t1.join();
    t2.join();
    return 0;
}