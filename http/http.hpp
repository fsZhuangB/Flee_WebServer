#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#include "../lock/locker.h"
class http_conn
{
    public:
    void process();
    bool write();

    public:
    int m_state; //读为0, 写为1
};
#endif