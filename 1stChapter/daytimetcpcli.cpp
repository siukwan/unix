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


int main(int argc,char **argv)
{
    int sockfd,n;
    char recvline[MAXLINE+1];
    sockaddr_in servaddr;//sockaddr_in in <sys/un.h>

    if(argc !=2)
        err_quit("usage: a.put <IPaddress>");

    if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
        err_sys("socket error");

    bzero(&servaddr,sizeof(servaddr));//对sockaddr结构体进行初始化清零

	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(13);//时间服务器的端口
	
	/*inet_pton in <arpa/inet.h>*/
	if( inet_pton( AF_INET,argv[1], &servaddr.sin_addr ) <=0 )
		err_sys("connect error");
	
	if(connect(sockfd, (SA *)&servaddr, sizeof(servaddr))<0)
		err_sys("connect error");
		
	/*read in <unistd.h>*/
	while( (n = read(sockfd, recvline, MAXLINE ) ) > 0 )
	{
		recvline[n]=0;
		if( fputs(recvline, stdout) == EOF )
			err_sys( "fputs error" );
	}
	
	if( n < 0 )
		err_sys( "read error" );

	exit( 0 );/*exit() in <stdlib.h>*/
    cout<<"Siukwan Program Finished"<<endl;
    return 0;
}
