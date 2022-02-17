#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>
#include "../lock/locker.h"

template <typename T>
class threadpool
{
    public:
    threadpool(int thread_number = 8, int max_request = 10000);
    ~threadpool();
    // 向请求队列中插入任务请求
    bool append(T *request, int state);
    bool append_p(T *request);

    private:
    // 工作线程回调函数，要设置为静态成员函数
    static void* worker(void* arg);
    void run();

    private:
    int m_thread_number;        //线程池中的线程数
    int m_max_requests;         //请求队列中允许的最大请求数
    pthread_t *m_threads;       //存放线程池中每个线程的tid
    std::list<T*> m_workqueue;  //请求队列
    locker m_queuelocker;       //请求队列互斥锁
    sem m_queuestat;            //信号量，用来通知是否有任务需要处理
    // connection_pool *m_connPool;//数据库
    int m_actor_model;          //模型切换
};

#endif