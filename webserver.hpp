#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <sys/epoll.h>

const int MAX_FD = 65536;           //最大文件描述符
const int MAX_EVENT_NUMBER = 10000; //最大事件数
const int TIMESLOT = 5;             //最小超时单位

class webserver
{
public:
    // 基础
    int m_port;
    char* m_root;
    int m_epollfd;

    // 数据库相关

    // 线程池相关

    // epoll_events 相关
    struct epoll_event events[MAX_EVENTS_NUM];
    
    int m_listenfd;

    
public:
    webserver() = default;
    ~webserver();
    void init(int port);
    void eventLoop();   /* epoll 主要循环 */
    void eventListen(); /* 处理监听事件 */
    void dealWithRead();/* 处理读事件 */
};

#endif