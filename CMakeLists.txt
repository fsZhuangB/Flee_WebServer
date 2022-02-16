# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# 项目信息
project (myserver)

# 指定生成目标
add_executable(myserver main.cpp webserver.cpp)