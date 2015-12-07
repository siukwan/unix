#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<iostream>
#include"memory.h"
#include<sys/socket.h>
#include<sys/un.h>
#include<arpa/inet.h>
using namespace std;

#define MAXLINE 4096
#define LISTENQ 1024
#define bzero(ptr,n) memset(ptr,0,n)
#define SA sockaddr
void err_sys(string str)
{
	cout<<str<<endl;
	exit(0);
}
//此函数是在程序发生错误时被调用    
//先输出字符串fmt，再根据errno输出错误原因（如果有的话），最后退出程序    
//注：在多线程程序中，错误原因可能不正确    
void error_quit(char *fmt, ...)    
{    
  /*  int res;    
    va_list list;    
    va_start(list, fmt);    
    res = vfprintf(stderr, fmt, list);    
    if( errno != 0 )    
		fprintf(stderr, " : %s", strerror(errno));
	fprintf(stderr, "\n", list);    
    va_end(list);    
	*/exit(1);    
}
/*包裹函数*/
int Socket(int family,int type ,int protocol)
{
	int n;
	if(( n = socket(family,type,protocol)) < 0)
		err_sys("socket error");
	return n;
}
void Connect(int fd, const struct sockaddr *sa, socklen_t salen)    
{    
    if (connect(fd, sa, salen) < 0)    
		error_quit("connect error");    
}    
			  
void Listen(int fd, int backlog)    
{    
	if (listen(fd, backlog) < 0)    
		error_quit("listen error");    
}    
						    
void Bind(int fd, const struct sockaddr *sa, socklen_t salen)    
{    
	if (bind(fd, sa, salen) < 0)    
		error_quit("bind error");    
}    
										  
int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)    
{    
	int n = accept(fd, sa, salenptr);    
	if ( n < 0)    
		error_quit("accept error");    
	return n;    
} 
void Write(int fd, void *ptr, size_t nbytes)    
{    
    if (write(fd, ptr, nbytes) != nbytes)    
		error_quit("write error");    
} 
void Close(int fd)    
{    
    if (close(fd) == -1)    
	error_quit("close error");    
}   
int main(int argc, char **argv)
{
	cout<<"Server start.."<<endl;
	int listenfd,connfd;
	sockaddr_in servaddr;
	char buff[MAXLINE];
	time_t ticks;

	listenfd = Socket(AF_INET,SOCK_STREAM,0);
	
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(13);/* daytime server*/

	Bind(listenfd,(SA *)&servaddr,sizeof(servaddr));
	Listen(listenfd,LISTENQ);
	cout<<"In the loop..."<<endl;
	for(;;)
	{
		connfd = Accept(listenfd,(SA *)NULL,NULL);
		ticks = time(NULL);
		snprintf(buff, sizeof(buff),"%.24s\r\n",ctime(&ticks));
		Write(connfd, buff, strlen(buff));

		Close(connfd);
	}

	return 0;
}
