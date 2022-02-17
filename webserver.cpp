#include "webserver.hpp"

webserver::~webserver()
{
    close(m_epollfd);
    close(m_listenfd);
}

void webserver::init(int port)
{
    m_port = port;
}

void webserver::thread_pool()
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
    struct epoll_event tempEvents;
    tempEvents.events = EPOLLIN | EPOLLET;
    tempEvents.data.fd = m_listenfd;
    ret = epoll_ctl(m_epollfd, EPOLL_CTL_ADD, m_listenfd, &tempEvents);
    assert(ret != -1);

    // 初始化epollfd
    http_conn::m_epollfd = m_epollfd;

    // 设置listenfd 文件描述符为非阻塞
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
            // 如果是读事件
            /* TODO: 反应堆待实现 */
            else if (events[i].events & EPOLLIN)
            {
                dealWithRead(sockfd);
            }
            else if (events[i].events & EPOLLOUT)
            {
                dealWithWrite(sockfd);
            }
            // 如果有异常情况
            // else if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
            // {
            //     //服务器端关闭连接，移除对应的定时器
            //     util_timer *timer = users_timer[sockfd].timer;
            //     deal_timer(timer, sockfd);
            // }
            // else
            // {
            //     int sockfd = events[i].data.fd;
            //     int n = read(sockfd, buffer, sizeof(buffer));
            //     if (n == 0)
            //     {
            //         int res = epoll_ctl(m_epollfd, EPOLL_CTL_DEL, sockfd, NULL);
            //         assert(res != -1);
            //         close(sockfd);
            //     }
            //     else if (n < 0)
            //     {
            //         close(sockfd);
            //         exit(1);
            //     }
            //     else
            //     {
            //         for (int j = 0; j < n; ++j)
            //         {
            //             buffer[j] = toupper(buffer[j]);
            //         }
            //         write(sockfd, buffer, n);
            //         write(STDOUT_FILENO, buffer,n);
            //     }
            // }

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

    // 设置cfd非阻塞
    setnonblocking(cfd);

    // struct epoll_event ev;
    // ev.data.fd = cfd;
    // ev.events = EPOLLIN | EPOLLET;
    // int ret = epoll_ctl(m_epollfd, EPOLL_CTL_ADD, cfd, &ev);
    // 加入到红黑树上
    addfd(m_epollfd, cfd, true);

    // 为该次连接（用户）初始化
    users[cfd].init(cfd, address);
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
