#include "threadpool.hpp"
#define MIN_WAIT_TASK_NUM 10            /*如果queue_size > MIN_WAIT_TASK_NUM 添加新的线程到线程池*/ 
#define DEFAULT_THREAD_VARY 10          /*每次创建和销毁线程的个数*/
threadpool_t* threadpool_create(int min_thr_num, int max_thr_num, int queue_max_size)
{
    int i;
    threadpool_t *pool = nullptr;
    do
    {
        pool = (threadpool_t *)malloc(sizeof(threadpool_t));
        if (pool == nullptr)
        {
            std::cout << "Wrong alloc to pool" << std::endl;
            break;
        }
        pool->min_thr_num = min_thr_num;
        pool->max_thr_num = max_thr_num;
        pool->busy_thr_num = 0;
        pool->live_thr_num = min_thr_num;
        pool->wait_exit_thr_num = 0;
        pool->queue_size = 0;
        pool->queue_front = 0;
        pool->queue_rear = 0;
        pool->queue_max_size = queue_max_size;
        pool->shutdown = false;

        pool->threads = (pthread_t*)malloc(sizeof(pthread_t) * max_thr_num);
        memset(pool->threads, 0, sizeof(pthread_t) * max_thr_num);
        pool->task_queue = (threadpool_task_t*)malloc(sizeof(threadpool_task_t) * queue_max_size);
        memset(pool->task_queue, 0, sizeof(threadpool_task_t) * queue_max_size);

    if (pthread_mutex_init(&pool->lock, NULL) != 0 ||
        pthread_mutex_init(&pool->thread_counter, NULL) != 0 ||
        pthread_cond_init(&pool->queue_not_full, NULL) != 0|| 
        pthread_cond_init(&pool->queue_not_empty, NULL) != 0)
        {
            std::cout << "init fail" << std::endl;
            break;
        }
    /* 创建子线程 */
    for (i = 0; i < min_thr_num; ++i)
    {
        /* pool 指向当前线程池 */
        pthread_create(&pool->threads[i], NULL, threadpool_thread, (void*) pool);
        std::cout << "Start thread " << pool->threads[i] << std::endl;
    }
    pthread_create(&pool->adjust_tid, NULL, adjust_thread, (void *)pool);

    /* 返回当前线程池 */
    return pool;
    } while (0);
    threadpool_free(pool);

    return nullptr;
}

int threadpool_add(threadpool_t *pool, void*(*function)(void *arg), void *arg)
{
    pthread_mutex_lock(&(pool->lock));

    /* ==为真，队列已经满， 调wait阻塞 */
    while ((pool->queue_size == pool->queue_max_size) && (!pool->shutdown)) {
        pthread_cond_wait(&(pool->queue_not_full), &(pool->lock));
    }

    if (pool->shutdown) {
        pthread_cond_broadcast(&(pool->queue_not_empty));
        pthread_mutex_unlock(&(pool->lock));
        return 0;
    }

    /* 清空 工作线程 调用的回调函数 的参数arg */
    if (pool->task_queue[pool->queue_rear].arg != NULL) {
        pool->task_queue[pool->queue_rear].arg = NULL;
    }

    /*添加任务到任务队列里*/
    pool->task_queue[pool->queue_rear].function = function;
    pool->task_queue[pool->queue_rear].arg = arg;
    /* 借助队尾指针，模拟环形队列 */
    pool->queue_rear = (pool->queue_rear + 1) % pool->queue_max_size;       /* 队尾指针移动, 模拟环形 */
    pool->queue_size++;

    /*添加完任务后，队列不为空，唤醒线程池中 等待处理任务的线程*/
    pthread_cond_signal(&(pool->queue_not_empty));
    pthread_mutex_unlock(&(pool->lock));

    return 0;
}
void* threadpool_thread(void* arg)
{
    threadpool_t* tpool = (threadpool_t*)arg;
    threadpool_task_t task;
    while (true)
    {
        pthread_mutex_lock(&tpool->lock);
        /* queue_size == 0时，说明没有任务，则调用wait阻塞在条件变量上 */
        while ((tpool->queue_size == 0) && (!tpool->shutdown))
        {
            std::cout << "Thread 0x" << pthread_self() << " is waiting" << std::endl;
            pthread_cond_wait(&tpool->queue_not_empty, &tpool->lock);
            if (tpool->wait_exit_thr_num > 0) {
                tpool->wait_exit_thr_num--;

                /*如果线程池里线程个数大于最小值时可以结束当前线程*/
                if (tpool->live_thr_num > tpool->min_thr_num) {
                    // printf("thread 0x%x is exiting\n", (unsigned int)pthread_self());
                    std::cout << "thread " << pthread_self() << "is exiting" << std::endl;
                    tpool->live_thr_num--;
                    pthread_mutex_unlock(&(tpool->lock));

                    pthread_exit(NULL);
                }
            }
        }
        /*如果指定了true，要关闭线程池里的每个线程，自行退出处理---销毁线程池*/
        if (tpool->shutdown) {
            pthread_mutex_unlock(&(tpool->lock));
            std::cout << "thread " << pthread_self() << "is exiting" << std::endl;
            pthread_detach(pthread_self());
            pthread_exit(NULL);     /* 线程自行结束 */
        }
        task.function = tpool->task_queue[tpool->queue_front].function;
        task.arg = tpool->task_queue[tpool->queue_front].arg;

        /* 出队，模拟环形队列 */
        tpool->queue_front = (tpool->queue_front + 1) % tpool->queue_max_size;
        tpool->queue_front--;

        pthread_cond_broadcast(&tpool->queue_not_full);
        pthread_mutex_unlock(&tpool->lock);

        /* 执行任务 */
        std::cout << "thread " << pthread_self() << " is working" << std::endl;
        pthread_mutex_lock(&tpool->thread_counter);
        tpool->busy_thr_num++;
        pthread_mutex_unlock(&tpool->thread_counter);
        *(task.function)(task.arg);

        /* 结束任务处理 */
        std::cout << "thread " << pthread_self() << " end working" << std::endl;
        pthread_mutex_lock(&tpool->thread_counter);
        tpool->busy_thr_num--;
        pthread_mutex_unlock(&tpool->thread_counter);
    }
    pthread_exit(NULL);
}

/* 管理线程 */
void *adjust_thread(void *threadpool)
{
    int i;
    threadpool_t *pool = (threadpool_t *)threadpool;
    while (!pool->shutdown) {

        sleep(10);                                    /*定时 对线程池管理*/

        pthread_mutex_lock(&(pool->lock));
        int queue_size = pool->queue_size;                      /* 关注 任务数 */
        int live_thr_num = pool->live_thr_num;                  /* 存活 线程数 */
        pthread_mutex_unlock(&(pool->lock));

        pthread_mutex_lock(&(pool->thread_counter));
        int busy_thr_num = pool->busy_thr_num;                  /* 忙着的线程数 */
        pthread_mutex_unlock(&(pool->thread_counter));

        /* 创建新线程 算法： 任务数大于最小线程池个数, 且存活的线程数少于最大线程个数时 如：30>=10 && 40<100*/
        if (queue_size >= MIN_WAIT_TASK_NUM && live_thr_num < pool->max_thr_num) {
            pthread_mutex_lock(&(pool->lock));  
            int add = 0;

            /*一次增加 DEFAULT_THREAD 个线程*/
            for (i = 0; i < pool->max_thr_num && add < DEFAULT_THREAD_VARY
                    && pool->live_thr_num < pool->max_thr_num; i++) {
                if (pool->threads[i] == 0 || !is_thread_alive(pool->threads[i])) {
                    pthread_create(&(pool->threads[i]), NULL, threadpool_thread, (void *)pool);
                    add++;
                    pool->live_thr_num++;
                }
            }

            pthread_mutex_unlock(&(pool->lock));
        }

        /* 销毁多余的空闲线程 算法：忙线程X2 小于 存活的线程数 且 存活的线程数 大于 最小线程数时*/
        if ((busy_thr_num * 2) < live_thr_num  &&  live_thr_num > pool->min_thr_num) {

            /* 一次销毁DEFAULT_THREAD个线程, 隨機10個即可 */
            pthread_mutex_lock(&(pool->lock));
            pool->wait_exit_thr_num = DEFAULT_THREAD_VARY;      /* 要销毁的线程数 设置为10 */
            pthread_mutex_unlock(&(pool->lock));

            for (i = 0; i < DEFAULT_THREAD_VARY; i++) {
                /* 通知处在空闲状态的线程, 他们会自行终止*/
                pthread_cond_signal(&(pool->queue_not_empty));
            }
        }
    }

    return NULL;
}

void *process(void *arg)
{
    std::cout << "thread 0x" << pthread_self() << "working on task ";
    sleep(1);                           //模拟 小---大写
    std::cout << "task" <<  pthread_self() << " is end" << std::endl;
    return nullptr;
}

int threadpool_free(threadpool_t *pool)
{
    if (pool == NULL) {
        return -1;
    }

    if (pool->task_queue) {
        free(pool->task_queue);
    }
    if (pool->threads) {
        free(pool->threads);
        pthread_mutex_lock(&(pool->lock));
        pthread_mutex_destroy(&(pool->lock));
        pthread_mutex_lock(&(pool->thread_counter));
        pthread_mutex_destroy(&(pool->thread_counter));
        pthread_cond_destroy(&(pool->queue_not_empty));
        pthread_cond_destroy(&(pool->queue_not_full));
    }
    free(pool);
    pool = NULL;

    return 0;
}

int threadpool_destroy(threadpool_t *pool)
{
    int i;
    if (pool == NULL) {
        return -1;
    }
    pool->shutdown = true;

    /*先销毁管理线程*/
    pthread_join(pool->adjust_tid, NULL);

    for (i = 0; i < pool->live_thr_num; i++) {
        /*通知所有的空闲线程*/
        pthread_cond_broadcast(&(pool->queue_not_empty));
    }
    for (i = 0; i < pool->live_thr_num; i++) {
        pthread_join(pool->threads[i], NULL);
    }
    threadpool_free(pool);

    return 0;
}

int is_thread_alive(pthread_t tid)
{
    int kill_rc = pthread_kill(tid, 0);     //发0号信号，测试线程是否存活
    if (kill_rc == ESRCH) {
        return false;
    }
    return true;
}