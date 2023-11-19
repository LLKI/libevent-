#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<event.h>
#include<fcntl.h>
#include<unistd.h>
//管道，当一边写入的时候，另一边就可读
//当监听到是事件满足条件(EV_READ)时，触发回调函数，读取数据
void fifo_read(evutil_socket_t fd, short events, void *arg)
{
	char buf[999]={0};
	int ret=read(fd,buf,sizeof(buf));//从fd读取放入buf
	if(ret==-1){perror("read");exit(1);}
	printf("从管道读取:%s\n",buf);
}
int main()
{
	//创建管道
	int ret=mkfifo("fifo.tmp",00700);
	if(ret==-1)//返回值不是0则创建错误
	{
		perror("mkfifo");
		exit(1);
	}
	//创建文件描述符fd
	int fd=open("fifo.tmp",O_RDONLY);//fd只能对管道进行读取
	if(fd==-1)
	{
		perror("open");
		exit(1);
	}
	//初始化事件集合
	event_init();
	
	//创建事件
	struct event ev;
	//绑定事件和fd以及回调函数 事件 文件描述符 事件类型 回调函数 回调函数参数
	event_set(&ev,fd,EV_READ | EV_PERSIST,fifo_read,NULL);
	//添加事件到集合中
	event_add(&ev,NULL);//null是指超时的时间
	//开始监听管道(死循环)
	event_dispatch();//当集合中没有事件可以监听则结束
	return 0;
}
