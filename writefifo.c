#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>


int main()
{
	
	//直接打开管道 默认阻塞
	int fd=open("fifo.tmp",O_WRONLY);
	if(fd==-1){perror("open");exit(1);}
	
	char buf[999]={0};
	while(1){
		//TODO
		scanf("%s",buf);
		int ret=write(fd,buf,strlen(buf));
		if(ret==-1){perror("write");exit(1);}
		if(!strcmp(buf,"bye")){break;}
		memset(buf,0,sizeof(buf));
	}
	
	return 0;
}
