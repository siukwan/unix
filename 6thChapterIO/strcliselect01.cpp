/*
P133 strcliselect01
*/
#include"func.h"
using namespace std;
/*
 * 第五章
 * 服务器关闭，客户端阻塞于stdin，输入字符串后，进行发送才发现服务器关闭，退出程序
 * 第六章
 * 服务器关闭，客户端的select检测到连接断开，直接检测到，然后退出程序
 * */
void str_cli_select01(FILE *fp,int sockfd)
{
	int maxfdp1;
	fd_set rset;
	char sendline[MAXLINE],recvline[MAXLINE];

	FD_ZERO(&rset);
	while(1)
	{
		FD_SET(fileno(fp), &rset);
		FD_SET(sockfd,&rset);
		maxfdp1 = max(fileno(fp),sockfd) +1;
		printf("进行select中...\n");
		Select(maxfdp1,&rset,NULL,NULL,NULL);

		//socket is readable
		if(FD_ISSET(sockfd,&rset))
		{
			if(read(sockfd,recvline,MAXLINE)==0)
				err_quit("str_cli:server terminated prematurely");
			printf("接收到的字符串：\n");
			Fputs(recvline,stdout);
		}

		//input is readable
		if(FD_ISSET(fileno(fp),&rset))
		{
			if(Fgets(sendline,MAXLINE,fp) == NULL)
				return ;
			printf("输入的字符串为：%s，进行发送...\n",sendline);
			Writen(sockfd,sendline,strlen(sendline));
		}
		memset(recvline,0,MAXLINE);
		memset(sendline,0,MAXLINE);
	}
}

