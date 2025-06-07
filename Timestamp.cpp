#include "Timestamp.hpp"
#include <time.h>

Timestamp::Timestamp():secondsSinceEpoch_(0){}

Timestamp::Timestamp(int64_t secondsSinceEpochArg):secondsSinceEpoch_(secondsSinceEpochArg){}

//获取现在的时间的Timestamp对象
Timestamp Timestamp::now(){
    return Timestamp(time(NULL));
}
//将时间转换成字符串形式
std::string Timestamp::toString(){
    char buf[128]={0};
    int64_t sec = secondsSinceEpoch_;
    struct tm* tp = localtime(&sec);
    snprintf(buf, 128, "%4d/%02d/%02d %02d:%02d:%02d",
        tp->tm_year+1900,
        tp->tm_mon+1,
        tp->tm_mday,
        tp->tm_hour,
        tp->tm_min,
        tp->tm_sec);
    return buf;
}
