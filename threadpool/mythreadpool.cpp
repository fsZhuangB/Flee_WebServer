#include "mythreadpool.hpp"

template <typename T>
threadpool<T>::threadpool(int thread_number, int max_request) : m_thread_number(thread_number), m_max_requests(max_request){
    if (thread_number <= 0 || max_request <= 0)
    {
        throw std::exception();
    }
    // 创建线程数组
    m_threads = new pthread_t[m_thread_number];
    if (!m_threads)
    {
        throw std::exception();
    }
    for (int i = 0; i < thread_number; ++i)
    {
        if (pthread_create(m_threads + i, NULL, worker, this) != 0)
        {
            delete [] m_threads;
            throw std::exception();
        }
        if (pthread_detach(m_threads[i]) != 0)
        {
            delete [] m_threads;
            throw std::exception();
        }
    }
}

template <typename T>
threadpool<T>::~threadpool()
{
    delete[] m_threads;
}

/**
 * @brief 
 * 添加任务进入任务队列
 * @tparam T 
 * @param request http_conn类的请求
 * @param state 0指读事件，1指写事件
 * @return true 正常添加
 * @return false 添加错误
 */
template <typename T>
bool threadpool<T>::append(T * request, int state)
{
    // 一上来要加锁
    m_queuelocker.lock();
    if (m_workqueue.size() >= m_max_requests)
    {
        m_queuelocker.unlock();
        return false;
    }
    request->m_state = state;
    m_workqueue.push_back(request);
    m_queuelocker.unlock();
    m_queuestat.P();
    return true;
}

/**
 * @deprecated
 */
template <typename T>
bool threadpool<T>::append_p(T *request)
{
    m_queuelocker.lock();
    if (m_workqueue.size() >= m_max_requests)
    {
        m_queuelocker.unlock();
        return false;
    }
    m_workqueue.push_back(request);
    m_queuelocker.unlock();
    // 通知可写
    m_queuestat.P();
    return true;
}

/**
 * @brief worker接收线程池的状态，并执行run函数
 * 
 * @tparam T 
 * @param arg 接收整个线程池的状态
 * @return void* 返回线程池
 */
template <typename T>
void* threadpool<T>::worker(void* arg)
{
    threadpool* pool = (threadpool*) arg;
    pool->run();
    return pool;
}

/**
 * @brief 工作线程从请求队列中取出某个任务进行处理。
 * 
 * @tparam T 
 */
template <typename T>
void threadpool<T>::run()
{
    while (true)
    {
        // 信号量V操作：
        // 1. 如果信号量大于0，则--
        // 2. 如果信号量等于0，则阻塞
        m_queuestat.V();

        // 唤醒后先加互斥锁
        m_queuelocker.lock();
        if (m_workqueue.empty())
        {
            m_queuelocker.unlock();
            continue;
        }
        // 从队列中取出任务
        T* request = m_workqueue.front();
        m_workqueue.pop_front();
        m_queuelocker.unlock();

        if (!request)
        { continue; }
        /**
         * @todo 增加模型选择
         */

        if (request->m_state == 0)
        {
            // 如果是读，调用process(模板类中的方法,这里是http类)进行处理
            request->process();
        }
        else
        {
            request->write();
        }
    }
}