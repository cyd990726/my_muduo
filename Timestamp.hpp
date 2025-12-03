#pragma once
#include <iostream>
#include <string>

// 定义时间戳对象

class Timestamp{
public:
    Timestamp();
    //防止发生隐式转换,对于单参数的构造函数要尤其注意这一点。
    explicit Timestamp(int64_t microSecondsSinceEpochArg);
    //获取现在的时间的Timestamp对象
    static Timestamp now();
    //将时间转换成字符串形式
    std::string toString();
private:
    //从这个纪元开始的秒数
    int64_t secondsSinceEpoch_;
};