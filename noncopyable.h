#pragma once
/**
 * 被继承以后，派生类对象可以正常的构造和析构，但是派生类对象无法进行拷贝构造和赋值操作。
 * 这是一个很重要的编程思想
 */
class noncopyable
{
public:
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&)=delete;

protected:
    //保证本类不能被构造和析构，但是不影响派生类的构造和析构
    noncopyable()=default;
    ~noncopyable()=default;

};
