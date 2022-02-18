#include "http.hpp"

int http_conn::m_epollfd = -1;
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
    event.events = ev | EPOLLET | EPOLLRDHUP;

    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}
HTTP_CODE http_conn::process_read()
{

}
/**
 * @brief
 * 该函数调用分别调用process_read和process_write完成报文解析和报文响应
 */
void http_conn::process()
{
    HTTP_CODE read_ret = process_read();
    // NO_REQUEST，表示请求不完整，需要继续接收请求数据
    if (read_ret == NO_REQUEST)
    {
        // 重新监听读事件
        modfd(m_epollfd, m_sockfd, EPOLLIN);
        return ;
    }
    //调用process_write完成报文响应
    bool write_ret = process_write(read_ret);
    if (!write_ret)
    {
        close_conn();
    }
    // 重置事件为EPOLLOUT，通知文件描述符可写
    modfd(m_epollfd, m_sockfd, EPOLLOUT);
}

bool http_conn::read()
{
    if( m_read_idx >= READ_BUFFER_SIZE )
    {
        return false;
    }

    int bytes_read = 0;
    while( true )
    {
        bytes_read = recv( m_sockfd, m_read_buf + m_read_idx, READ_BUFFER_SIZE - m_read_idx, 0 );
        if ( bytes_read == -1 )
        {
            if( errno == EAGAIN || errno == EWOULDBLOCK )
            {
                break;
            }
            return false;
        }
        else if ( bytes_read == 0 )
        {
            return false;
        }

        m_read_idx += bytes_read;
    }
    return true;

}

bool http_conn::write()
{
    /**
     * @todo 需要重新实现
     * 
     */
    int ret = send(m_sockfd, m_read_buf, sizeof(m_read_buf), 0);
    if (ret > 0)
    {
        std::cout << "send fd= " << m_sockfd << " with "<< m_read_buf << std::endl;
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
    int error = 0;
    socklen_t len = sizeof(error);
    getsockopt(m_sockfd, SOL_SOCKET, SO_ERROR, &error, &len);
    int reuse = 1;
    setsockopt( m_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof( reuse ) );
    addfd(m_epollfd, sockfd, true);
    m_user_count++;

    init();
}

void http_conn::init()
{
    m_check_state = CHECK_STATE_REQUESTLINE;
    m_method = GET;
    m_start_line = 0;
    m_checked_idx = 0;
    m_read_idx = 0;
    
    memset( m_read_buf, '\0', READ_BUFFER_SIZE );
}


