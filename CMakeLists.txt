cmake_minimum_required(VERSION 3.0)
project(mymuduo)

# 最终编译成动态库，设置库的输出路径
set(LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/)

# 设置调试信息，以及启动C++11语言标准
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")

# 定义参与编译的源代码文件集合
aux_source_directory(. SRC_LIST)

# 编译动态库
add_library(my_muduo SHARED ${SRC_LIST})
