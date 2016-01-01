/*
1. g++  -o client tcpcli.cpp func_err.cpp func_wrap.cpp
2. ./client 127.0.0.1 (or ./clent XXX.XXX.XXX.XXX，XXX.XXX.XXX.XXX is the server's ip )
*/
#include"func.h"
using namespace std;

//主函数
int main(int argc,char **argv)
{
	//开启5个socket
	int sockfd,n;

	sockaddr_in servaddr;//sockaddr_in in <sys/un.h>

	char ip_default[]="127.0.0.1";
	char *ip_address;
	if(argc !=2)
	{
		printf("usage: ./client <IPaddress> For an example: ./client 192.168.1.111\n");
		printf("Now,the default IP address is 127.0.0.1\n");
		ip_address=ip_default;
	}
	else
		ip_address = argv[1];
	/*socket*/
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	/*init */
	bzero(&servaddr,sizeof(servaddr));//对sockaddr结构体进行初始化清零
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(MYPORT);//时间服务器的端口

	/*inet_pton in <arpa/inet.h>*/
	if( inet_pton( AF_INET,ip_address, &servaddr.sin_addr ) <=0 )
		err_sys("connect error");

	Connect(sockfd,(SA *) &servaddr, sizeof(servaddr));

	//只对第一个socket进行操作
	str_cli_select01(stdin,sockfd);

	exit( 0 );/*exit() in <stdlib.h>*/

	return 0;
}
