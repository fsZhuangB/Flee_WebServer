#include "http.hpp"

//对文件描述符设置非阻塞
int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

//将内核事件表注册读事件，ET模式，选择开启EPOLLONESHOT
void addfd(int epollfd, int fd, bool one_shot)
{
    epoll_event event;
    event.data.fd = fd;

    // 开启了EPOLLRDHUP检测对端是否关闭
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    // 开启EPOLLONESHOT，让事件仅能触发一次
    if (one_shot)
    { event.events |= EPOLLONESHOT; }
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

//从内核时间表删除描述符
void removefd(int epollfd, int fd)
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
    close(fd);
}

//将事件重置为EPOLLONESHOT，并添加新的事件
void modfd(int epollfd, int fd, int ev)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = ev | EPOLLET | EPOLLONESHOT | EPOLLRDHUP;

    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

/**
 * @brief
 * process
 */
void http_conn::process()
{
    memset( m_read_buf, '\0', READ_BUFFER_SIZE );
    while (1)
    {
        int ret = recv(m_sockfd, m_read_buf, READ_BUFFER_SIZE-1, 0);
        removefd(m_epollfd, m_sockfd);

        if( ret == 0 )
        {
            close( m_sockfd );
            std::cout << "foreiner closed the connection" << std::endl;
            break;
        }
        else if (ret < 0)
        {
            if (errno == EAGAIN)
            {
                modfd(m_epollfd, m_sockfd, EPOLLIN);
                break;
            }
        }
        else
        {
            std::cout<< "get content:" <<  m_read_buf << std::endl;
            modfd(m_epollfd, m_sockfd, EPOLLOUT);
        }
    }
}

bool http_conn::write()
{
    int ret = send(m_sockfd, m_read_buf, sizeof(m_read_buf), 0);
    removefd(m_epollfd, m_sockfd);
    if (ret > 0)
    {
        std::cout << "send fd=" << m_sockfd << "with "<< m_read_buf << std::endl;
        modfd(m_epollfd, m_sockfd, EPOLLIN);
    }
    else
    {
        close(m_sockfd);
        std::cout << "send error\n" << std::endl;
        return false;
    }
    return true;
}

//初始化连接,外部调用初始化套接字地址
void http_conn::init(int sockfd, const sockaddr_in &addr)
{
    m_sockfd = sockfd;
    m_address = addr;   
    addfd(m_epollfd, sockfd, true);

    // init();
}


