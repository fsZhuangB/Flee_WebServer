#include "webserver.hpp"

int main(int argc, char* argv[])
{
    webserver myserver;

    // 初始化监听端口号9006
    myserver.init(9006);

    // 初始化线程池
    myserver.thread_pool();

    // 初始化监听
    myserver.eventListen();

    // 运行
    myserver.eventLoop();

    return 0;
}