#include <iostream>
#include "Timestamp.hpp"
#include "Logger.hpp"

//获取唯一实例对象
Logger& Logger::instance(){
    static Logger logger;
    return logger;
}
//设置日志级别
void Logger::setLogLevel(LogLevel level){
    this->logLevel_ = level;
}
//写日志
void Logger::log(std::string msg){
    switch (this->logLevel_){
        case LogLevel::INFO:
            std::cout<< "\033[32m[INFO]\033[0m\n";
            break;
        case LogLevel::ERROR:
            std::cout<< "\033[31m[ERROR]\033[0m\n";
            break;
        case LogLevel::DEBUG:
            std::cout<< "\033[32m[DEBUG]\033[0m\n";
            break;
        case LogLevel::FATAL:
            std::cout<< "\033[31m[FATAL]\033[0m\n";
            break;
        default:
            break;
    }
    //打印时间和msg
    std::cout<<Timestamp::now().toString() << ":" << msg << std::endl;
}