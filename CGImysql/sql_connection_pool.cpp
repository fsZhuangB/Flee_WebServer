#include <mysql/mysql.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <list>
#include <pthread.h>
#include <iostream>
#include "sql_connection_pool.h"

connectionPool* connectionPool::getInstance()
{
	// 线程安全定义方式
	static connection_pool connPool;
	return &connPool;
}

connRAII::connRAII(MYSQL **con, connectionPool *connPool)
{
	*con = connPool->getConnection();

	connRAII = *con;
	poolRAII = connPool;
}

connRAII::~connRAII()
{
	connRAII->releaseConnection(connRAII);
}