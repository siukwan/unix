#include<stdio.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
//简单的服务器实现
int main()
{
	printf("server is running...\n");
	int listenFd,connectFd;
	
	listenFd = socket(AF_INET,SOCK_STREAM,0);//创建一个套接字

	//设置地址和监听端口
	sockaddr_in servAddr;//netinet/in.h
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");//把字符串的ip转换成网络序
	servAddr.sin_port = htons(8800);//arpa/inet.h 把short型从主机序转换成网络序
	
	bind(listenFd,(sockaddr*)&servAddr,sizeof(servAddr));//需要把sockaddr_in强制转换成sockaddr
	int backlog;
	listen(listenFd,backlog);

	printf("listening...\n");
	while(1)
	{
		printf("connecting...\n");

		sockaddr_in cliAddr;
		socklen_t addrLength = sizeof(sockaddr_in);
		connectFd = accept(listenFd,(sockaddr*)&cliAddr,&addrLength);
		printf("connect success!\n");
	}

	return 0;
}
