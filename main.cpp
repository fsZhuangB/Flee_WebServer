#include "webserver.hpp"

int main(int argc, char* argv[])
{
    webserver myserver;

    // 初始化监听端口号1234
    myserver.init(1234);

    // 初始化线程池
    myserver.thread_pool();

    // 初始化监听
    myserver.eventListen();

    // 运行
    myserver.eventLoop();

    return 0;
}