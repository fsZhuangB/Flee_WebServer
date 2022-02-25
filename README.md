# Flee_WebServer
Flee WebServer是一个高性能webserver，使用epoll实现IO多路复用，ET模式，IO模型为Reactor模式，同时使用多线程充分利用多核CPU，并使用线程池避免线程频繁创建销毁的开销，用了状态机解析HTTP请求，支持GET、POST请求，支持长连接，支持注册登陆。正在逐步更新中。

## 更新日志
- [x] 实现lock.h封装类
- [x] 实现Reactor模式
- [x] 实现线程池
- [x] 实现状态机解析HTTP请求
- [x] 解决请求服务器上大文件的Bug
- [x] 解决数据库同步校验内存泄漏
- [x] 使用RAII机制优化数据库连接的获取与释放
- [x] 完成初步压力测试
- [ ] 增加请求视频文件的页面
- [ ] 实现长连接并测试
- [ ] 使用优雅关闭连接
- [ ] 实现注册登陆功能
- [ ] 实现同步/异步日志系统
- [ ] 实现定时器处理非活动连接

