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

    // 设置listenfd 文件描述符为非阻塞
}

void webserver::eventLoop()
{
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
            // else if (events[i].events & EPOLLIN)
            // {
            //     dealWithRead(sockfd);
            // }
            // else if (events[i].events & EPOLLOUT)
            // {
            //     dealWithWrite(sockfd);
            // }
            else
            {
                int sockfd = events[i].data.fd;
                int n = read(sockfd, buffer, sizeof(buffer));
                if (n == 0)
                {
                    int res = epoll_ctl(m_epollfd, EPOLL_CTL_DEL, sockfd, NULL);
                    assert(res != -1);
                    close(sockfd);
                }
                else if (n < 0)
                {
                    close(sockfd);
                    exit(1);
                }
                else
                {
                    for (int j = 0; j < n; ++j)
                    {
                        buffer[j] = toupper(buffer[j]);
                    }
                    write(sockfd, buffer, n);
                    write(STDOUT_FILENO, buffer,n);
                }
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

    // 设置cfd非阻塞
    int flag = fcntl(cfd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(cfd, F_SETFL, flag);

    struct epoll_event ev;
    ev.data.fd = cfd;
    ev.events = EPOLLIN | EPOLLET;
    int ret = epoll_ctl(m_epollfd, EPOLL_CTL_ADD, cfd, &ev);
    assert(ret != -1);
}