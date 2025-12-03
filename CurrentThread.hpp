#pragma once

#include <sys/syscall.h>
#include <unistd.h>

namespace CurrentThread {
    // 线程局部存储变量，保存线程ID.
    // __thread是GCC提供的线程局部存储关键字
    extern __thread int t_cachedTid;
    void cacheTid();
    inline int tid(){
        if(__builtin_expect(t_cachedTid == 0, 0)){
            cacheTid();
        }
        return t_cachedTid;
    }

}
