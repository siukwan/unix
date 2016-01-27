/**
 * 2016年1月27日
 * CGI服务器远离
 *
 */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <iostream>
using namespace std;
int main( int argc, char* argv[])
{

	if(argc <= 2)
	{
		printf("usage: %s ip_address port_number\n",argv[0]);
		return 1;
	}

	const char* ip = argv[1];
	int port = atoi(argv[2]);

	sockaddr_in address;
	bzero(&address, sizeof(address));//初始化清空
	address.sin_family = AF_INET;
	inet_pton(AF_INET,ip,&address.sin_addr);
	address.sin_port = htons(port);

	int sock = socket (PF_INET,SOCK_STREAM,0);
	assert(sock >=0);//如果sock小于0，则中断退出

	cout<<"bind"<<endl;
	int ret = bind(sock, (sockaddr*)&address, sizeof(address));
	assert(ret!=-1);

	cout<<"listen"<<endl;
	ret = listen(sock , 5);
	assert(ret != -1);

	sockaddr_in client;
	socklen_t client_addrlength = sizeof(client);
	int connfd = accept (sock,(sockaddr*)&client, &client_addrlength);
	if(connfd<0)
		printf("errno is : %d\n",errno);
	else
	{
		close(STDOUT_FILENO);
		dup(connfd);
		printf("CGI-program\n");
		close(connfd);
	}

	close(sock);
	return 0;
}
