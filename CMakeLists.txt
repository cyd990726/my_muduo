cmake_minimum_required(VERSION 3.0)
project(mymuduo)

# # 最终编译成动态库，设置库的输出路径
# set(LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/)
# 设置静态库和动态库的输出路径
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)  # 静态库
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)  # 动态库


# 设置调试信息，以及启动C++11语言标准
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")

# 定义参与编译的源代码文件集合
aux_source_directory(. SRC_LIST)

# 编译动态库
add_library(my_muduo SHARED ${SRC_LIST})

# 测试
enable_testing()
# 测试实例Timestamp
add_executable(test_timestamp ${PROJECT_SOURCE_DIR}/test/test_timestamp.cpp ${PROJECT_SOURCE_DIR}/Timestamp.cpp)
target_link_libraries(test_timestamp gtest pthread)
add_test(NAME TimestampTest COMMAND test_timestamp)

# 测试实例inetAddress.cpp
add_executable(InetAddressTest ${PROJECT_SOURCE_DIR}/test/test_inetaddress.cpp ${PROJECT_SOURCE_DIR}/InetAddress.cpp)
target_link_libraries(InetAddressTest gtest pthread)
add_test(NAME InetAddressTest COMMAND InetAddressTest)
