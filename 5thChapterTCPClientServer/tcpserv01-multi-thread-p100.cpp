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
/*Write "n" bytes to a descriptor*/
ssize_t writen(int fd, const void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr=(char *) vptr;
	nleft=n;
	while(nleft>0)
	{
		if((nwritten = write(fd,ptr,nleft))<=0)
		{
			if(errno == EINTR)
				nwritten = 0;/* and call write() again*/
			else
				return (-1);
		}
		nleft -= nwritten;
		ptr+= nwritten;
	}
	return (n);
}

void Writen(int fd,void *ptr,size_t nbytes)
{
	if(writen(fd,ptr,nbytes)!=nbytes)
		err_sys("writen	error");
}

pid_t Fork(void)
{
	pid_t pid=fork();
	if(pid == -1 )
		err_quit("fork error");
	return pid;
}
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
	socklen_t   chilen;          /*the size of cliadrr */
	
	/*socket*/
	listenfd = Socket(AF_INET,SOCK_STREAM,0);

	/*set ip and port*/
	bzero(&servaddr, sizeof(servaddr));	
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(13);/* daytime server*/
	
	/*bind*/
	Bind(listenfd,(SA *)&servaddr,sizeof(servaddr));
	
	/*listen*/
	Listen(listenfd,LISTENQ);

	cout<<"In the loop..."<<endl;
	for(;;)
	{
		cout<<"Wait for connecting..."<<endl;
		
		/*accept (blocking)*/
		chilen=sizeof(cliaddr);
		connfd = Accept(listenfd,(SA *)&cliaddr, &chilen);/*read the info in the cliaddr*/
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
