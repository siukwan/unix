/*
P137 strcliselect02
*/
#include"func.h"
using namespace std;
/*
 * 第五章
 * 服务器关闭，客户端阻塞于stdin，输入字符串后，进行发送才发现服务器关闭，退出程序
 * 第六章
 * 服务器关闭，客户端的select检测到连接断开，直接检测到，然后退出程序
 * */
void str_cli_select02(FILE *fp,int sockfd)
{
	//stdineof为输入eof标志符
	bool stdineof=false;
	int maxfdp1;
	fd_set rset;

	char buf[MAXLINE];
	FD_ZERO(&rset);
	while(1)
	{
		//如果标准输入还没读到EOF，继续把fp的描述符设置为监听
		if(!stdineof)
			FD_SET(fileno(fp), &rset);
		FD_SET(sockfd,&rset);
		maxfdp1 = max(fileno(fp),sockfd) +1;
		printf("\n进行select中...\n");
		Select(maxfdp1,&rset,NULL,NULL,NULL);

		//socket is readable
		if(FD_ISSET(sockfd,&rset))
		{
			printf("select：socket\n");
			if(read(sockfd,buf,MAXLINE)==0)
			{
				//如果stdin输入结束，并且sockfd读取也结束，则return
				if(stdineof)
					return ;
				else
					err_quit("str_cli:server terminated prematurely");
			}
			//打印输出
			printf("接收到的字符串：\n");
			Write(fileno(stdout),buf,strlen(buf));

		}

		//input is readable
		if(FD_ISSET(fileno(fp),&rset))
		{
			printf("select：stdin\n");
			//文件已经读取完毕，关闭socket的发送功能
			if(read(fileno(fp),buf,MAXLINE) == 0)
			{
				stdineof=1;
				Shutdown(sockfd,SHUT_WR);//send FIN
				FD_CLR(fileno(fp),&rset);
				continue;
			}
			printf("输入的字符串为：%s（buf自动换行）进行发送...\n",buf);
			Writen(sockfd,buf,strlen(buf));
		}
		memset(buf,0,MAXLINE);
	}
}
