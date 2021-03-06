/*
1. g++ tcpdaytimecli.cpp -o client
2. ./client 127.0.0.1   (or ./clent XXX.XXX.XXX.XXX  XXX.XXX.XXX.XXX is the server's ip  )
*/
#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<memory.h>
#include<unistd.h>
#include<iostream>
#include<errno.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<arpa/inet.h>
#include<syslog.h>

using namespace std;

#define SA sockaddr
#define MAXLINE 4096
#define bzero(ptr,n) memset(ptr,0,n)
#define recv(a,b,c,d) recvfrom(a,b,c,d,0,0)
#define send(a,b,c,d) sendto(a,b,c,d,0,0)
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
int Socket(int family ,int type , int protocol)
{
	int n;
	if((n=socket(family, type, protocol)) <0)
		err_sys("socket error");
	return n;
}
void Connect(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if( connect(fd, sa, salen) < 0 )
		err_quit("connect error");
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
		if( (nwritten = write(fd, ptr, nleft ))  <= 0 )
		{
			if(errno == EINTR)
				nwritten = 0;
			else
				return (-1);
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
	return (n);
}
void Writen(int fd, void *ptr, size_t nbytes)
{
	if(writen(fd, ptr, nbytes) != nbytes )
		err_sys("written error");
}

static int read_cnt;
static char *read_ptr;
static char read_buf[MAXLINE];


void Fputs(const char *ptr, FILE *stream)
{
	if(fputs(ptr,stream)==EOF)
		err_quit("fputs error");
}
char *Fgets(char *ptr,int n,FILE *stream)
{
	char *rptr = fgets(ptr, n ,stream);
	if(rptr== NULL && ferror(stream))
		err_quit("fgets error");
	return rptr;
}
void str_cli(FILE *fp, int sockfd)
{
	char sendline[MAXLINE], recvline[MAXLINE];
	printf("\n请输入字符串：\n");
	while(Fgets(sendline, MAXLINE, fp)!=NULL)
	{
		//printf("std输入的字符串为：%s\n",sendline);
		//通过writen把数据发送到服务器
		Writen(sockfd, sendline, strlen(sendline));
		printf("发送给服务器...\n");
		int n=read(sockfd,recvline,MAXLINE);
		if(n==0)
			err_quit("str_cli:server terminated prematurely\n");
		else
		{
			printf("读取的数据大小为%d:\n",n);
		}
		//if(Readline(sockfd, recvline, MAXLINE)==0)
		//	err_quit("str_cli:server terminated prematurely");
		
		//输出到stdout
		Fputs(recvline,stdout);

		//提示输入，并清空
		printf("\n请输入字符串：\n");
		memset(recvline,0,MAXLINE);
		memset(sendline,0,MAXLINE);
	}
}


//主函数
int main(int argc,char **argv)
{
    int sockfd,n;
    
    sockaddr_in servaddr;//sockaddr_in in <sys/un.h>

    if(argc !=2)
        err_quit("usage: ./client <IPaddress>  For an example: ./client 127.0.0.1");
	/*socket*/
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	
	/*init */
    bzero(&servaddr,sizeof(servaddr));//对sockaddr结构体进行初始化清零

	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(13);//时间服务器的端口
	
	/*inet_pton in <arpa/inet.h>*/
	if( inet_pton( AF_INET,argv[1], &servaddr.sin_addr ) <=0 )
		err_sys("connect error");
	
	Connect(sockfd,(SA *) &servaddr, sizeof(servaddr));
	
	str_cli(stdin,sockfd);

	exit( 0 );/*exit() in <stdlib.h>*/
	return 0;
}
