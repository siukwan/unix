/*
 * poll版本的服务器
1. g++  -o server_poll tcpserv.cpp func_err.cpp func_wrap.cpp
2. use the port 9877 (MYPORT),no sudo anymore
*/

#include"func.h"
using namespace std;
int main(int argc, char **argv)
{
	cout<<"Server start.."<<endl;
	int         listenfd,connfd;
	pid_t       childpid;        /*child process id*/
	sockaddr_in servaddr,cliaddr;/*define cliaddr*/
	socklen_t   clilen;          /*the size of cliadrr */
	
	/*socket*/
	//调用socket，制定通信协议类型
	listenfd = Socket(AF_INET,SOCK_STREAM,0);

	/*set ip and port*/
	//设置ip和端口
	bzero(&servaddr, sizeof(servaddr));	
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(MYPORT);
	
	/*bind*/
	//绑定
	Bind(listenfd,(SA *)&servaddr,sizeof(servaddr));
	
	/*listen*/
	//进行监听
	Listen(listenfd,LISTENQ);

	//最大文件描述符号
	int maxfd = listenfd;
	//初始化初始下标
	int maxIdx=-1;

	//初始化客户数组为－1
	int client[FD_SETSIZE];
	for(int i=0;i<FD_SETSIZE;i++)
	{
		client[i]=-1;
	}

	fd_set rset,allset;
	//初始化监听集合
	FD_ZERO(&allset);
	//把listenfd添加到监听集合
	FD_SET(listenfd,&allset);

	cout<<"In the loop..."<<endl;
	while(1)
	{
		rset = allset;
		//对rset进行select
		int nReady = Select(maxfd+1,&rset,NULL,NULL,NULL);
		
		//如果监听就绪，表示有客户进行连接
		if(FD_ISSET(listenfd,&rset))
		{
			printf("有客户进行连接...\n");
			clilen = sizeof(cliaddr);
			connfd = Accept(listenfd, (SA*)&cliaddr,&clilen);

			//找出第一个－1的index
			int firstIdx;
			for(firstIdx=0;firstIdx<FD_SETSIZE;firstIdx++)
			{
				if(client[firstIdx] == -1)
				{
					client[firstIdx]=connfd;
					break;
				}
			}

			if(firstIdx == FD_SETSIZE)
				err_quit("too many clients");

			//添加到监听集合
			FD_SET(connfd,&allset);

			maxfd=max(maxfd,connfd);
			maxIdx=max(maxIdx,firstIdx);

			//没有可读的描述符
			if(--nReady <=0)
				continue;
		}

		//遍历所有的客户端
		for( int i=0;i<=maxIdx;i++)
		{
			int sockfd;
			char buf[MAXLINE];
			if((sockfd = client[i]) == -1)
				continue;
			if(FD_ISSET(sockfd,&rset))
			{
				printf("套接字可读...\n");
				//客户端关闭连接
				if(read(sockfd,buf,MAXLINE)==0)
				{
					Close(sockfd);
					//移除监听
					FD_CLR(sockfd,&allset);
					client[i]=-1;
				}
				else
					Writen(sockfd,buf,strlen(buf));
				memset(buf,0,strlen(buf));
				//没有可读的描述符
				if(--nReady<=0)
					break;
			}
		}

	}

	return 0;
}
