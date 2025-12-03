# my_muduo
自己实现的muduo网络库

## 安装gtest框架
* Ubuntu/Debian
sudo apt-get install libgtest-dev libgmock-dev

* macOS (Homebrew)
brew install googletest

## 笔记
### 以继承noncopyable类的方式让不能拷贝构造和赋值的类都无法执行拷贝和赋值
定义了一个基类noncopyable，里面拷贝构造和赋值函数被delete了，这样继承它的类就都无法拷贝构造和赋值了。同时将默认构造和默认析构函数设置为protected。这样的好处是可以防止noncopyable直接使用。但是又不影响子类的构造和析构。
