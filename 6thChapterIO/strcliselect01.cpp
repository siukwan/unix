/*
P133 strcliselect01
*/
#include"func.h"
using namespace std;
//二进制版本
void str_cli_select01(FILE *fp,int sockfd)
{
	int maxfdp1;
	fd_set rset;
	char sendline[MAXLINE],recvline[MAXLINE];

	FD_ZERO(&rset);
	printf("\n请输入字符串：\n");
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
			Writen(sockfd,sendline,strlen(sendline));
		}
		memset(recvline,0,MAXLINE);
		memset(sendline,0,MAXLINE);
	}
}
