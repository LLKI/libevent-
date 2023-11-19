#include<stdio.h>
#include<signal.h>
#include<event.h>

int signal_count=0;
void signal_handler(evutil_socket_t fd, short events, void *arg)
{
	struct event *ev=(struct event *)arg;
	signal_count++;
	printf("第%d次收到信号:%d\n",signal_count,fd);//输出fd为2，因为SIGINT（Ctrl+C）与信号编号 2 相关联
	if(signal_count>=3)
	{
		//把事件ev从集合中删除
		event_del(ev);
	}
	
}
int main()
{
	//创建事件集合(第二种方法)
	struct event_base *base=event_base_new();
	//创建事件
	struct event ev;
	
	//绑定信号和事件 事件 事件集合 文件描述符(这里是信号ctrl+c) 事件类型
	event_assign(&ev,base,SIGINT,EV_SIGNAL|EV_PERSIST,signal_handler,&ev);
	//添加事件到集合中
	event_add(&ev,NULL);
	//监听集合
//	event_dispatch();//默认监听的是全局集合
	event_base_dispatch(base);//我们自己创建了集合，应该使用这个函数指定监听的集合
	
	//释放集合
	event_base_free(base);
	return 0;
}
