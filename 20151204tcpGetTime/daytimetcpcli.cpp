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
#define bzero(ptr,n) memset(ptr,0,n)
#define recv(a,b,c,d) recvfrom(a,b,c,d,0,0)
#define send(a,b,c,d) sendto(a,b,c,d,0,0)

void err_quit(string str)
{
	cout<<str<<endl;
	exit(0);
}
void err_sys(string str)
{
	cout<<str<<endl;
	exit(0);
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
