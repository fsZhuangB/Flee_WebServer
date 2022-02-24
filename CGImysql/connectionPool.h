#ifndef _CONNECTIONPOOL_
#define _CONNECTIONPOOL_

#include <stdio.h>
#include <list>
#include <mysql/mysql.h>
#include <error.h>
#include <string.h>
#include <iostream>
#include <string>
#include "../lock/locker.h"
//#include "../log/log.h"

using namespace std;

/**
 * @brief 懒汉单例模式
 * 
 */
class connectionPool
{
    public:
    MYSQL* getConnection(); // 获取数据库连接
    bool releaseConnection(MYSQL*); // 释放数据库连接
    void destroyPool(); // 销毁连接池

    void init(string user, string passwd, string url, string databaseName, int port, int maxConnection);

    public:
    string url;			 //主机地址
	string port;		 //数据库端口号
	string user;		 //登陆数据库用户名
	string passwd;	 //登陆数据库密码
	string databaseName; //使用数据库名
	int closeLog;	//日志开关

    public:
    static connectionPool* getInstance();

    private:
    int maxConn; // 最大连接数目
	int curConn;  //当前已使用的连接数
	int freeConn; //当前空闲的连接数
    locker lock;
    sem haveFree; // 阻塞在是否有剩余空闲连接上
    list<MYSQL*> connList;

    private:
    connectionPool();
    ~connectionPool();
    connectionPool(const connectionPool&) = delete;
    connectionPool& operator=(const connectionPool&) = delete;
};

class connRAII
{
    public:
    connRAII(MYSQL **con, connectionPool *connPool);
	~connRAII();

    private:
	MYSQL *mysqlConn;
	connectionPool *poolRAII;
};

#endif
