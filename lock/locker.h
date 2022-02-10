#ifndef LOCKER_H
#define LOCKER_H
#include <exception>
#include <pthread.h>
#include <semaphore.h>

class sem 
{
    public:
    /**
     * @brief Construct a new sem object, sem number = 0
     * 
     */
    sem() 
    {
        if (sem_init(&sem_num, 0, 0) != 0)
        {
            throw std::exception();
        }
    }
    sem(int num)
    {
        if (sem_init(&sem_num, 0, num) != 0)
        {
            throw std::exception();
        }
    }
    ~sem()
    {
        sem_destroy(&sem_num);
    }
    bool V()
    {
        return sem_wait(&sem_num) == 0;
    }
    bool P()
    {
        return sem_post(&sem_num) == 0;
    }
    private:
    sem_t sem_num;
};

class locker
{
    public:
    locker()
    {
        if (pthread_mutex_init(&mutex, NULL) != 0)
        {
            throw std::exception();
        }
    }
    ~locker()
    {
        pthread_mutex_destroy(&mutex);
    }
    bool lock()
    {
        return pthread_mutex_lock(&mutex) == 0;
    }
    bool unlock()
    {
        return pthread_mutex_unlock(&mutex) == 0;
    }
    pthread_mutex_t *get()
    {
        return &mutex;
    }
    private:
    pthread_mutex_t mutex;
};


class cond_t
{
    public:
    cond_t()
    {
        if (pthread_cond_init(&m_cond, NULL) != 0)
        {
            //pthread_mutex_destroy(&m_mutex);
            throw std::exception();
        }
    }
    ~cond_t()
    {
        pthread_cond_destroy(&m_cond);
    }
    bool wait(pthread_mutex_t *m_mutex)
    {
        int ret = 0;
        //pthread_mutex_lock(&m_mutex);
        ret = pthread_cond_wait(&m_cond, m_mutex);
        //pthread_mutex_unlock(&m_mutex);
        return ret == 0;
    }

    bool timewait(pthread_mutex_t *m_mutex, struct timespec t)
    {
        int ret = 0;
        //pthread_mutex_lock(&m_mutex);
        ret = pthread_cond_timedwait(&m_cond, m_mutex, &t);
        //pthread_mutex_unlock(&m_mutex);
        return ret == 0;
    }

    bool signal()
    {
        return pthread_cond_signal(&m_cond) == 0;
    }
    bool broadcast()
    {
        return pthread_cond_broadcast(&m_cond) == 0;
    }
    private:
    pthread_cond_t m_cond;
};

#endif