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
	pollfd client[OPEN_MAX];
	//0好设置为listenfd
	client[0].fd=listenfd;
	//设置为普通数据可读
	client[0].events = POLLRDNORM;
	for(int i=1;i<OPEN_MAX;i++)
	{
		client[i].fd=-1;
	}


	cout<<"In the loop..."<<endl;
	while(1)
	{
		//对rset进行select
		int nReady = Poll(client,maxfd+1,INFTIM);
		
		//如果监听就绪，表示有客户进行连接
		if(client[0].revents & POLLRDNORM)
		{
			printf("有客户进行连接...\n");
			clilen = sizeof(cliaddr);
			connfd = Accept(listenfd, (SA*)&cliaddr,&clilen);

			//找出第一个－1的index
			int firstIdx;
			for(firstIdx=1;firstIdx<OPEN_MAX;firstIdx++)
			{
				if(client[firstIdx].fd == -1)
				{
					client[firstIdx].fd=connfd;
					break;
				}
			}

			if(firstIdx == OPEN_MAX)
				err_quit("too many clients");

			//添加到监听集合
			client[firstIdx].events=POLLRDNORM;

			maxfd=max(maxfd,connfd);
			maxIdx=max(maxIdx,firstIdx);

			//没有可读的描述符
			if(--nReady <=0)
				continue;
		}

		//遍历所有的客户端
		for( int i=1;i<=maxIdx;i++)
		{
			int sockfd;
			char buf[MAXLINE];
			if((sockfd = client[i].fd) == -1)
				continue;
			if(client[i].revents & (POLLRDNORM|POLLERR))
			{
				printf("套接字可读...\n");
				int result = read(sockfd,buf,MAXLINE);
				//客户端reset连接
				if(result<0)
				{
					if(errno == ECONNRESET)
					{
						Close(sockfd);
						client[i].fd = -1;
					}
					else
						err_sys("read error");
				}
				//客户端关闭连接
				else if(result==0)
				{
					Close(sockfd);
					client[i].fd=-1;
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
