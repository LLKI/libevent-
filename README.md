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
#### 实现思路
1. 创建管道:mkfifo
2. 使用open打开管道，创建文件描述符fd
3. event_init():初始化事件集合
4. struct event ev:创建事件
5. event_set:初始化事件(绑定ev和fd,指定事件类型，绑定回调函数)，本质是调用event_assign
6. event_add:把事件添加到集合中
7. event_dispatch:监听管道

`wirtefifo.c`打开管道，等待输入，将输入内容写入管道<br>
![image](https://github.com/LLKI/libevent-/assets/92627079/540db1e9-ab84-483b-ae80-bd23fff053c2)<br>
### 信号事件
只有`signalevent.c`这个文件<br>
其实与IO事件类似，只是将IO事件中的"等待输入"改为了"接收信号"，接收到信号的时候调用回调函数
![image](https://github.com/LLKI/libevent-/assets/92627079/078e4a72-fcb2-40c6-88b0-bb70079d0555)<br>
#### 实现思路
1. 创建事件集合(第二种方法):event_base_new
2. struct event ev:创建事件
3. event_assign:初始化事件
4. event_add:把事件添加到集合中
5. event_base_dispatch:指定监听的事件集合
6. event_base_free:释放集合
### 服务器高并发编程
基于libevent实现的并发服务器，具有简单的接收功能<br>
文件包括`server.c`和`client.c`,服务端可以同时连接多个客户端<br>
服务端运行<br>
![image](https://github.com/LLKI/libevent-/assets/92627079/a4a2275d-b5f0-416b-b6ea-b75e3621c825)<br>
客户端运行<br>
![image](https://github.com/LLKI/libevent-/assets/92627079/31c33a4a-7b81-499e-9411-c35eda2eb86f)<br>

#### 服务端实现思路
1. 创建socket
2. bind:绑定信息
3. listen:监听是否有客户端连接
4. accept:用于接受连接,返回文件描述符(fd)用于读写socket

其实`evconnlistener_new_bind`这个已经将上面的4个步骤都封装好了,重点放在定义事件集合(包括事件)以及回调函数上就好
