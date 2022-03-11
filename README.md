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
- [x] 实现注册登陆功能
- [x] 增加请求视频文件的页面
- [ ] 为线程池增加动态扩容机制
- [ ] 使用优雅关闭连接
- [ ] 实现同步/异步日志系统
- [ ] 实现定时器处理非活动连接

## 项目总结

1.   [线程池设计总结](https://github.com/fsZhuangB/Flee_WebServer/blob/main/%E7%BA%BF%E7%A8%8B%E6%B1%A0%E8%AE%BE%E8%AE%A1.md)

2.   [并发模型总结](https://github.com/fsZhuangB/Flee_WebServer/blob/main/%E5%B9%B6%E5%8F%91%E6%A8%A1%E5%9E%8B%E6%80%BB%E7%BB%93.md)
