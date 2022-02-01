#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP
#include <pthread.h>
#include <memory.h>
#include <iostream>
#include <unistd.h>

typedef struct 
{
    void* (*function)(void *); /* 回调函数 */
    void* arg; /* 函数参数 */
} threadpool_task_t;

class threadpool_t {
    public:
    // 本结构体锁
    pthread_mutex_t lock;
    // 记录忙线程个数锁
    pthread_mutex_t thread_counter;
    pthread_cond_t queue_not_full; 
    pthread_cond_t queue_not_empty;

    pthread_t* threads; /* 存放线程池中每个线程的tid */
    pthread_t adjust_tid; /* 管理者线程 */ 
    int min_thr_num;
    int max_thr_num;
    int live_thr_num;
    int busy_thr_num;
    int wait_exit_thr_num; /* 要销毁线程数目 */
    
    threadpool_task_t *task_queue;      /* 任务队列(数组首地址) */
    int queue_front;
    int queue_rear;
    int queue_size;
    int queue_max_size;

    bool shutdown; /* 标志位，线程池使用状态 */
};



threadpool_t* threadpool_create(int min_thr_num, int max_thr_num, int queue_max_size);
int threadpool_add(threadpool_t* pool, void*(*function)(void *arg), void* arg);
int threadpool_destroy(threadpool_t* pool);
int threadpool_free(threadpool_t *pool);

/* 工作线程回调函数 */
void* threadpool_thread(void* arg);
/* 调整管理回调函数 */
void* adjust_thread(void* arg);
/* 任务回调函数，处理任务 */
void *process(void *arg);

int is_thread_alive(pthread_t tid);

#endif