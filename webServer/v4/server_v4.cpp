#include<stdlib.h>
#include<stdio.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include"response.h"
#include "get_time.h"
#include "http_session.h"
#include "http_protocol.h"
//简单的服务器实现
int main()
{
//	printf("%s",GLOBAL_HTTP_RESPONSE);
	printf("server is running...\n");
	int listenFd,connectFd;
	int res;
	listenFd = socket(AF_INET,SOCK_STREAM,0);//创建一个套接字

	//设置地址和监听端口
	sockaddr_in servAddr;//netinet/in.hi
	bzero(&servAddr,sizeof(servAddr));//string.h
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = INADDR_ANY;// inet_addr("127.0.0.1");//把字符串的ip转换成网络序
	servAddr.sin_port = htons(8800);//arpa/inet.h 把short型从主机序转换成网络序

	/* 如果服务器终止后,服务器可以第二次快速启动而不用等待一段时间  */
	int flag;
	setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));
	res = bind(listenFd,(sockaddr*)&servAddr,sizeof(servAddr));//需要把sockaddr_in强制转换成sockaddr
	if(res<0)
	{
		printf("bind error, check the port.\n");
		exit(1);//stdlib.h
	}
	int backlog;
	listen(listenFd,backlog);

	printf("listening...\n");
	while(1)
	{
		fd_set readFds;//读就绪的fd集
		FD_ZERO(&readFds);//清零
		FD_SET(listenFd,&readFds);//添加轮询
		int nfds = listenFd+1;
		printf("selecting...\n");
		int ready;
		//select 连接
		ready = select(nfds,&readFds,NULL,NULL,NULL);
		if(ready ==0)
		{
			printf("\nselect time out!\n");
			continue;
		}
		else if(ready==-1)
		{
			printf("select error\n");
		}
		if(!FD_ISSET(listenFd,&readFds))
		{
			printf("no ready!\n");
			continue;
		}
		sockaddr_in cliAddr;
		socklen_t addrLength = sizeof(sockaddr_in);
		connectFd = accept(listenFd,(sockaddr*)&cliAddr,&addrLength);
		printf("connect success!\n");
		

		sockaddr_in client_addr;//netinet/in.hi
		if (http_session(&connectFd, &client_addr) == -1) {/*处理客户端请求*/
			perror("http_session() error. in server_v4.c");
			printf("pid %d loss connection to %s\n", getpid(), inet_ntoa(
					client_addr.sin_addr));
		}
		//显示完毕后，关闭fd
		close(connectFd);//unistd.h
	}

	return 0;
}
