#pragma once

#include "noncopyable.h"
#include <string>

//方便用户使用，将器定义为宏函数。
//LOG_INFO("%s %d", arg1,arg2)
//注意：这里的##__VA_ARGS__是可变参数列表字符串的形式
#define LOG_INFO(logmsgFormat, ...)\
    do{\
        Logger & logger = Logger::instance();\
        logger.setLogLevel(INFO);\
        char buf[1024]={0};\
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__);\
        logger.log(buf);\
    }while(0)

#define LOG_FATAL(logmsgFormat, ...)\
    do{\
        Logger & logger = Logger::instance();\
        logger.setLogLevel(FATAL);\
        char buf[1024]={0};\
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__);\
        logger.log(buf);\
    }while(0)
    
#define LOG_ERROR(logmsgFormat, ...)\
    do{\
        Logger & logger = Logger::instance();\
        logger.setLogLevel(ERROR);\
        char buf[1024]={0};\
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__);\
        logger.log(buf);\
    }while(0)

//这里考虑debug信息只在调试的时候打印，而在运行模式下不打印
#ifdef MUDEBUG
    #define LOG_DEBUG(logmsgFormat, ...)\
        do{\
            Logger & logger = Logger::instance();\
            logger.setLogLevel(DEBUG);\
            char buf[1024]={0};\
            snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__);\
            logger.log(buf);\
        }while(0)
#else
    #define LOG_DEBUG(logmsgFormat, ...)
#endif

// 定义日志的级别：INFO ERROR FATAL DEBUG
enum LogLevel
{
    INFO, //普通信息
    ERROR, //错误信息
    FATAL, //core信息
    DEBUG //调试信息
};

//定义日志类，注意是一个单例对象。
class Logger : noncopyable{

    public:
    //获取唯一实例对象
    static Logger& instance();
    //设置日志级别
    void setLogLevel(int level);
    //写日志
    void log(std::string msg);

    private:
    //日志级别
    int logLevel_;
    Logger(){}
};