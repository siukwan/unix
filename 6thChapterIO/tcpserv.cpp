/*
1. g++  -o client tcpcli.cpp func_err.cpp func_wrap.cpp
2. use the port 9877 (MYPORT),no sudo anymore
*/

#include"func.h"
using namespace std;

//回射服务器
void str_echo(int sockfd)
{
	ssize_t n=0;
	char    buf[MAXLINE];
//	printf("准备读区数据...\n");
	while(1)
	{
		memset(buf,0,MAXLINE);
//		printf("进入while(1)循环..\n");
		while( (n=read(sockfd, buf, MAXLINE)) >0 )
		{
//			printf("接收到的内容：%s准备进行发送...\n",buf);
			Writen(sockfd ,buf, n);
//			printf("已经发送：%s\n",buf);
			memset(buf,0,MAXLINE);
		}

		if( n<0 && errno == EINTR )
		{
//			printf("error\n");
			continue;
		}
		else if(n<0)
			err_sys("str_echo: read error");
		else if(n==0)
		{
//			printf("n大小为：%ld，循环结束\n\n",n);
			return;
		}

	}
}


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
	//
	Signal(SIGCHLD,sig_chld);
	cout<<"In the loop..."<<endl;
	for(;;)
	{
		cout<<"Wait for connecting..."<<endl;
		
		/*accept (blocking)*/
		clilen=sizeof(cliaddr);
		connfd = Accept(listenfd,(SA *)&cliaddr, &clilen);/*read the info in the cliaddr*/
		cout<<"Forking..."<<endl;
		if( (childpid = Fork()) == 0 )
		{/*if pid == 0, it is the child process*/
			/*close listening socket*/
			Close(listenfd);

			/*process the request , return whatever server recieved*/
			str_echo(connfd);

			/*child process have finished the work ,then exits*/
		
			exit(0);
		}
		printf("派生子线程%d\n",childpid);
		cout<<"Forked!"<<endl;
		Close(connfd);
	}

	return 0;
}
