#include "webserver.hpp"

int main(int argc, char* argv[])
{
    webserver myserver;
    myserver.init(1234);

    // 初始化监听
    myserver.eventListen();

    // 运行
    myserver.eventLoop();

    return 0;
}