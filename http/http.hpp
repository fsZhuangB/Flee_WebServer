#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <string.h>
#include <iostream>

#include "../lock/locker.h"

int setnonblocking(int fd);
void addfd(int epollfd, int fd, bool one_shot);
void removefd(int epollfd, int fd);
void modfd(int epollfd, int fd, int ev);

class http_conn
{
    public:
    static const int FILENAME_LEN = 200;
    static const int READ_BUFFER_SIZE = 2048;
    static const int WRITE_BUFFER_SIZE = 1024; 
    enum METHOD { GET = 0, POST, HEAD, PUT, DELETE, TRACE, OPTIONS, CONNECT, PATCH };
    enum CHECK_STATE { CHECK_STATE_REQUESTLINE = 0, CHECK_STATE_HEADER, CHECK_STATE_CONTENT };
    enum HTTP_CODE { NO_REQUEST, GET_REQUEST, BAD_REQUEST, NO_RESOURCE, FORBIDDEN_REQUEST, FILE_REQUEST, INTERNAL_ERROR, CLOSED_CONNECTION }; 
    enum LINE_STATUS { LINE_OK = 0, LINE_BAD, LINE_OPEN };

    public:
    void process(); /* 外部调用 */
    bool write(); /* 工作线程用来写的函数 */
    bool read(); /* 工作线程用来读的函数 */
    void init(int sockfd, const sockaddr_in &addr); /* 初始化连接,外部调用初始化套接字地址 */
    void close_conn( bool real_close = true ); /* 关闭链接 */

    private:
    void init(); /* 内部私有变量初始化函数 */

    HTTP_CODE process_read(); /* 主状态机函数 */
    bool process_write( HTTP_CODE ret );

    HTTP_CODE parse_request_line( char* text ); /* 解析请求行 */
    HTTP_CODE parse_headers( char* text ); /* 解析请求头 */
    HTTP_CODE parse_content( char* text ); /* 解析请求内容 （POST）*/
    HTTP_CODE do_request(); /* 分析请求文件 */
    LINE_STATUS parse_line(); /* 从状态机函数 */
    char* get_line() { return m_read_buf + m_start_line; }

    private:
    void unmap();
    bool add_response( const char* format, ... );
    bool add_content( const char* content );
    bool add_status_line( int status, const char* title );
    bool add_headers( int content_length );
    bool add_content_length( int content_length );
    bool add_linger();
    bool add_blank_line();

    public:
    int m_state; //读为0, 写为1

    private:
    int m_sockfd;
    sockaddr_in m_address;
    char m_read_buf[READ_BUFFER_SIZE];
    int m_read_idx;
    int m_checked_idx;
    int m_start_line;
    char m_write_buf[ WRITE_BUFFER_SIZE ];
    int m_write_idx; // 要写回到客户端的长度

    char m_real_file[ FILENAME_LEN ];
    char* m_url;
    char* m_version;
    char* m_host;
    int m_content_length;
    bool m_linger; // 是否支持长连接

    CHECK_STATE m_check_state;
    METHOD m_method;

    char* m_file_address; /* mmap返回内存映射首地址 */
    struct stat m_file_stat;
    struct iovec m_iv[2]; /* 向量数组 */
    int m_iv_count;

    public:
    static int m_epollfd;
    static int m_user_count; // 连接数量
};
#endif