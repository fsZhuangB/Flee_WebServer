#include "webserver.hpp"

void show_error( int connfd, const char* info )
{
    printf( "%s", info );
    send( connfd, info, strlen( info ), 0 );
    close( connfd );
}

webserver::webserver()
{
    //http_conn类对象
    users = new http_conn[MAX_FD];
}

webserver::~webserver()
{
    close(m_epollfd);
    close(m_listenfd);
    delete[] users;
    delete m_pool;
}

void webserver::init(int port, std::string user, std::string passwd, std::string dbName, int sqlNum, int threadNum)
{
    m_port = port;
    user = user;
    passwd = passwd;
    dbName = dbName;
    sqlNum = sqlNum;
    m_thread_num = threadNum;
}

/**
 * @brief 初始化connection pool对象
 * 
 */
void webserver::connPoolInit()
{
    connPool = connectionPool::getInstance();
    connPool->init(user, passwd, "localhost", dbName, 3306, sqlNum);

    // http对象初始化数据库表
    users->initSqlConn(connPool);
}

void webserver::threadPool()
{
    // 初始化线程池
    m_pool = new threadpool<http_conn>();
}

void webserver::eventListen()
{
    m_listenfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(m_listenfd >= 0);

    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(m_port);
    address.sin_family = AF_INET;

    // 设置端口复用
    int flag = 1;
    setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag));
    ret = bind(m_listenfd, (struct sockaddr*)&address, sizeof(address));
    assert(ret >= 0);

    ret = listen(m_listenfd, 5);
    assert(ret >= 0);

    // 创建epoll
    m_epollfd = epoll_create(5);
    assert(m_epollfd != -1);

    // 添加到监听队列
    addfd(m_epollfd, m_listenfd, false);

    // 初始化epollfd
    http_conn::m_epollfd = m_epollfd;

}

void webserver::eventLoop()
{
    /**
     * @todo: add timeout here
     * 
     */
    bool stop = false;
    while (!stop)
    {
        int nready = epoll_wait(m_epollfd, events, MAX_EVENTS_NUM, -1);
        if (nready < 0 && errno != EINTR)
        {
            printf("%s", "epoll failure");
            break;
        }
        for (int i = 0; i < nready; ++i)
        {
            // 获取对应的文件描述符
            int sockfd = events[i].data.fd;
            // 如果是连接事件
            if (sockfd == m_listenfd)
            {
                bool flag = dealWithClientData();
                if (!flag)
                { continue; }
            }
            else if (events[i].events & ( EPOLLRDHUP | EPOLLHUP | EPOLLERR ))
            {
                users[sockfd].close_conn();
            }
            // 如果是读事件
            else if (events[i].events & EPOLLIN)
            {
                dealWithRead(sockfd);
            }
            // 如果是写事件
            else if (events[i].events & EPOLLOUT)
            {
                dealWithWrite(sockfd);
            }
        }
    }
    close(m_listenfd);
}

bool webserver::dealWithClientData()
{
    struct sockaddr_in address;
    socklen_t client_addr_len = sizeof(address);
    int cfd = accept(m_listenfd, (struct sockaddr*)&address, &client_addr_len);
    assert(cfd != -1);
    if (http_conn::m_user_count >= MAX_FD)
    {
        show_error( cfd, "Internal server busy" );
        return false;
    }

    // 将cfd加入到红黑树上
    addfd(m_epollfd, cfd, true);

    // 为该次连接（用户）初始化
    users[cfd].init(cfd, address);
    return true;
}

void webserver::dealWithRead(int sockfd) /* 处理读事件 */
{
    // reactor模式
    // 将socket可读事件放入请求队列，交给工作线程处理，0代表读事件
    m_pool->append(users + sockfd, 0);
}

void webserver::dealWithWrite(int sockfd)
{
    // reactor模式
    // 将socket可写事件放入请求队列，交给工作线程处理，1代表写事件
    m_pool->append(users + sockfd, 1);
}
