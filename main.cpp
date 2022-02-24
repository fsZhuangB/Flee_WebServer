#include "webserver.hpp"

int main(int argc, char* argv[])
{
    std::string user = "root";
    std::string passwd = "root";
    std::string dbName = "dbname";

    webserver myserver;

    // 初始化监听端口号9006，默认threadNum=8，数据连接num=8
    myserver.init(9006, user, passwd, dbName, 8, 8);

    // 初始化线程池
    myserver.threadPool();

    // 初始化数据库连接池
    myserver.connPoolInit();

    // 初始化监听
    myserver.eventListen();

    // 运行
    myserver.eventLoop();

    return 0;
}