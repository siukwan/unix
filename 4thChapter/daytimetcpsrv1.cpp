/*
 * use the 1stChapter client program
 * record the client's ip and port,and display them.

1. g++ tcpdaytimesrv.cpp -o server
2. sudo ./server  
PS:because it use the port 13,it must be the root.
*/

#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<iostream>
#include<memory.h>
#include<unistd.h>
#include<errno.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<arpa/inet.h>
#include<syslog.h>
using namespace std;

#define MAXLINE 4096
#define LISTENQ 1024
#define bzero(ptr,n) memset(ptr,0,n)
#define SA sockaddr
int daemon_proc;
void err_doit(int errnoflag, int level ,const char *fmt, va_list ap)
{
	int errno_save,n;
	char buf[MAXLINE+1];

	errno_save = errno;
#ifdef HAVE_VSNPRINTF
	vsnprintf(buf,MAXLINE, fmt , ap);
 #else
	vsprintf(buf,fmt,ap);
#endif
	n=strlen(buf);
	if(errnoflag) 
		snprintf(buf+n,MAXLINE-n,": %s",strerror(errno_save));
	strcat(buf,"\n");	
	if(daemon_proc)
	{
		syslog(level,buf);
	}
	else
	{
		fflush(stdout);
		fputs(buf,stderr);
		fflush(stderr);
	}
}
void err_sys(const char *fmt,...)
{
	va_list ap;
	va_start(ap,fmt);
	err_doit(1,LOG_ERR,fmt,ap);
	va_end(ap);
	exit(1);
}
void err_quit(const char *fmt,...)
{
	 va_list ap;
	 va_start(ap,fmt);
	 err_doit(0,LOG_ERR, fmt,ap);
	 va_end(ap);
	 exit(1);
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
		err_quit("connect error");    
}    
			  
void Listen(int fd, int backlog)    
{    
	if (listen(fd, backlog) < 0)    
		err_quit("listen error");    
}    
						    
void Bind(int fd, const struct sockaddr *sa, socklen_t salen)    
{    
	if (bind(fd, sa, salen) < 0)
		err_quit("bind error,if you use the port 13,you may use sudo");    
}    
										  
int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)    
{    
	int n = accept(fd, sa, salenptr);    
	if ( n < 0)    
		err_quit("accept error");    
	return n;    
} 
void Write(int fd, void *ptr, size_t nbytes)    
{    
    if (write(fd, ptr, nbytes) != nbytes)    
		err_quit("write error");    
} 
void Close(int fd)    
{    
    if (close(fd) == -1)    
	err_quit("close error");    
}   
int main(int argc, char **argv)
{
	cout<<"Server start.."<<endl;
	int listenfd,connfd;
	sockaddr_in servaddr,cliaddr;/*define cliaddr*/
	char buff[MAXLINE];
	time_t ticks;

	socklen_t len;/*the size of cliadrr */

	listenfd = Socket(AF_INET,SOCK_STREAM,0);
	bzero(&servaddr, sizeof(servaddr));	
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(13);/* daytime server*/

	Bind(listenfd,(SA *)&servaddr,sizeof(servaddr));
	Listen(listenfd,LISTENQ);
	cout<<"In the loop..."<<endl;
	for(;;)
	{
		cout<<"Wait for connecting..."<<endl;
		len=sizeof(cliaddr);
		connfd = Accept(listenfd,(SA *)&cliaddr, &len);/*read the info in the cliaddr*/
		/*display client's ip and port*/
		printf("connection from %s, port %d\n",
				inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),
				ntohs(cliaddr.sin_port));

		ticks = time(NULL);
		snprintf(buff, sizeof(buff),"%.24s\r\n",ctime(&ticks));
		Write(connfd, buff, strlen(buff));

		Close(connfd);
	}

	return 0;
}
