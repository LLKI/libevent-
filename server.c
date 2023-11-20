#include<stdio.h>
#include<stdlib.h>
#include<event.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<event2/listener.h>

/*
基于libevent的高并发服务器
流程:
socket:创建空socket
bind:绑定信息
listen:监听是否有客户端连接
accept:用于接受连接,返回文件描述符(fd)用于读写socket
*/

void read_cb(struct bufferevent *bev,void *ctx)
{
	int fd=*(int*)ctx;
	char buf[128]={0};
	size_t ret=bufferevent_read(bev,buf,sizeof(buf));
	if(ret<0)
	{
		printf("bufferevent_read error\n");
	}
	else
	{
		printf("read from %d:%s\n",fd,buf);
	}
}

void event_cb(struct bufferevent *bev,short what,void *ctx)
{
	int fd=*(int*)ctx;
	if(what & BEV_EVENT_EOF)
	{
		printf("客户端%d下线\n",fd);
		bufferevent_free(bev);//释放bev对象
	}
	else
	{
		printf("未知错误\n");
	}
}

void listener_cb(struct evconnlistener *listener,evutil_socket_t fd,struct sockaddr *addr,int socklen,void *arg)
{
	printf("接收到%d连接请求并连接成功,此回调函数被调用\n",fd);
	struct event_base *base=arg;
	//从fd读取数据
	//1.可以设置为阻塞状态,再用read()读取
	//2.使用bufferevent缓冲区
	//参数：事件集合 fd(代表TCP连接) opinion(这里设置释放bufferevent对象就关闭连接)
	struct bufferevent *bev=bufferevent_socket_new(base,fd,BEV_OPT_CLOSE_ON_FREE);
	if(bev==NULL)
	{
		printf("bufferevent_socket_new error\n");
		exit(1);
	}
	evutil_socket_t *fd_ptr = (evutil_socket_t *)malloc(sizeof(evutil_socket_t));
	*fd_ptr = fd;
	//给bufferevent设置回调函数
	//参数：bufferevent对象，读事件回调函数 写事件回调函数 其他事件回调函数 参数
	bufferevent_setcb(bev,read_cb,NULL,event_cb,fd_ptr);
	
	//使能bufferevent对象
	bufferevent_enable(bev,EV_READ);
	
}
int main()
{
	//创建事件集合
	struct event_base *base=event_base_new();
	if(base==NULL)
	{
		printf("event_base_new error\n");
		exit(1);
	}
	
	struct sockaddr_in server_addr;
	memset(&server_addr,0,sizeof(server_addr));
	
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(8000);
	server_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	
	//创建socket 绑定 监听 接受连接
	//创建监听对象，在指定的地址上监听TCP连接
	//参数：事件集合 有连接时调用的函数(回调函数) 回调函数参数 释放监听对象时关闭socket | 端口重复使用 监听队列长度 绑定的信息
	struct evconnlistener *listener= evconnlistener_new_bind(base,listener_cb,base,
		LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE,10,(struct sockaddr*)&server_addr,sizeof(server_addr));
	if(listener==NULL)
	{
		printf("evconnlistener_new_bind error\n");
		exit(1);
	}
	
	//监听
	event_base_dispatch(base);
	
	//释放监听对象
	evconnlistener_free(listener);
	
	//释放集合
	event_base_free(base);
	
	return 0;
}
