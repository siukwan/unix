#include<stdio.h>
#include<sys/socket.h>
//简单的服务器实现
int main()
{
	printf("server is running...\n");
	int listenFd,connectFd;

	//设置地址和监听端口
	sockaddr servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htol("localhost");
	servAddr.sin_port = 8800;
	
	bind(listenFd,(SA*)&servAddr,sizeof(servAddr));
	int backlog;
	listen(listenFd,backlog);

	printf("listening...\n");
	while(1)
	{
		printf("connecting...\n");
		connfd = accept(listenFd);
		printf("connect success!\n");
	}

	return 0;
}
