# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# 项目信息
project (myserver)

link_directories(http)
link_directories(threadpool)
# 添加 math 子目录
add_subdirectory(http)
add_subdirectory(threadpool)

# 添加链接库
target_link_libraries(${PROJECT_NAME} http threadpool)

# 指定生成目标
add_executable(${PROJECT_NAME} main.cpp webserver.cpp)


