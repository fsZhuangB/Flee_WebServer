#include <mysql/mysql.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <list>
#include <pthread.h>
#include <iostream>
#include "connectionPool.h"

connectionPool* connectionPool::getInstance()
{
	// 线程安全定义方式
	static connection_pool connPool;
	return &connPool;
}

connectionPool::connectionPool()
{
	curConn = 0;  //当前已使用的连接数
	freeConn = 0; //当前空闲的连接数
}

void connectionPool::init(string user, string passwd, string url, string databaseName, int port, int maxConnection)
{
	user = user;
	passwd = passwd;
	url = url;
	port = port;
	databaseName = databaseName;

	for (int i = 0; i < maxConnection; ++i)
	{
		MYSQL *con = nullptr;
		con = mysql_init(con);
		if (con == nullptr)
		{
			std::cout << "MySQL INIT Error" << std::endl;
			exit(1);
		}
		con = mysql_real_connect(con, url.c_str(), user.c_str(), passwd.c_str(), databaseName.c_str(), port, NULL, 0);
		if (con == NULL)
		{
			std::cout << "MySQL CONNECT Error" << std::endl;
			exit(1);
		}
		connList.push_back(con);
		++freeConn;
	}
	// 初始化信号量为最大free connect数目
	haveFree = sem(freeConn);
	maxConn = freeConn;
}

MYSQL* connectionPool::getConnection() {
    if (connList.size() == 0)
    { return nullptr; }
    MYSQL* con = nullptr;
    haveFree.V();
    lock.lock();
    con = connList.front();
    connList.pop_front();
    --freeConn;
    ++curConn;

    lock.unlock();
    return con;
}

bool connectionPool::releaseConnection(MYSQL *con) {
    if (con == nullptr)
    { return false; }
    // 放回线程池
    lock.lock();
    connList.push_back(con);
    ++freeConn;
    --curConn;
    lock.unlock();

    haveFree.P();
    return true;
}

// 销毁连接池
void connectionPool::destroyPool() {
    lock.lock();
    if (connList.size() > 0)
    {
        list<MYSQL *>::iterator it;
        for (it = connList.begin(); it != connList.end(); ++it)
        {
            MYSQL *con = *it;
            mysql_close(con);
        }
        m_CurConn = 0;
        m_FreeConn = 0;
        connList.clear();
    }
    lock.unlock();
}

connectionPool::~connectionPool()
{
    destroyPool();
}

connRAII::connRAII(MYSQL **con, connectionPool *connPool)
{
	*con = connPool->getConnection();

	mysqlConn = *con;
	poolRAII = connPool;
}

connRAII::~connRAII()
{
	connRAII->releaseConnection(connRAII);
}