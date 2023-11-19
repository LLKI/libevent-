# libevent
## libevent介绍
>Libevent 是一个用C语言编写的、轻量级的开源高性能事件通知库，主要有以下几个亮点：事件驱动（ event-driven），高性能;轻量级，专注于网络，不如 ACE 那么臃肿庞大；源代码相当精炼、易读；跨平台，支持 Windows、 Linux、 *BSD 和 Mac Os；支持多种 I/O 多路复用技术， epoll、 poll、 dev/poll、 select 和 kqueue 等；支持 I/O，定时器和信号等事件；注册事件优先级。
Libevent 已经被广泛的应用，作为底层的网络库；比如 memcached、 Vomit、 Nylon、 Netchat等等。(百度百科)
## 特点
>事件驱动，高性能；
轻量级，专注于网络；
跨平台，支持 Windows、Linux、Mac Os等；
支持多种 I/O多路复用技术， epoll、poll、dev/poll、select 和kqueue 等；
支持 I/O，定时器和信号等事件；
## 组成
>事件管理包括各种IO（socket）、定时器、信号等事件，也是libevent应用最广的模块；
缓存管理是指evbuffer功能；
DNS是libevent提供的一个异步DNS查询功能；
HTTP是libevent的一个轻量级http实现，包括服务器和客户端
## 案例
### IO事件
当管道中一边写入时，另一及即可读<br>
文件包括`eventlisten.c`和`wirtefifo.c`<br><br>
`eventlisten.c`创建管道，并持续监听事件集合，当发生写入事件时，调用回调函数读取写入内容并输出<br>
![image](https://github.com/LLKI/libevent-/assets/92627079/cef72e67-6e8c-4911-9f6e-b90ea46027d3)<br>
#### api调用顺序(具体参数使用Source Insight查看源码)
1. 创建管道`mkfifo`
2. 使用open打开管道，创建文件描述符fd
3. event_init():初始化事件集合
4. struct event ev:创建事件
5. event_set:初始化事件(绑定ev和fd,指定事件类型，绑定回调函数)
6. event_add:把事件添加到集合中
7. event_dispatch:监听管道

`wirtefifo.c`打开管道，等待输入，将输入内容写入管道<br>
![image](https://github.com/LLKI/libevent-/assets/92627079/540db1e9-ab84-483b-ae80-bd23fff053c2)<br>
### signal事件
### 服务器高并发
