/*
 * use the 1stChapter client program
 * record the client's ip and port,and display them.

1. g++ tcpdaytimesrv.cpp -o server
2. sudo ./server  
PS:because it use the port 13,it must be the root.
*/

#include"func.h"
using namespace std;

//20160101,二进制传输版本
void str_echo(int sockfd)
{
	ssize_t n;
	args m_args;
	result m_result;
	while(1)
	{
		if((n=read(sockfd,&m_args,MAXLINE))==0)
			return;
		m_result.sum = m_args.arg1+m_args.arg2;
		Writen(sockfd,&m_result,sizeof(result));
	}
}



int main(int argc, char **argv)
{
	cout<<"Server start.."<<endl;
	int         listenfd,connfd;
	pid_t       childpid;        /*child process id*/
	sockaddr_in servaddr,cliaddr;/*define cliaddr*/
	socklen_t   clilen;          /*the size of cliadrr */
	
	/*socket*/
	//调用socket，制定通信协议类型
	listenfd = Socket(AF_INET,SOCK_STREAM,0);

	/*set ip and port*/
	//设置ip和端口
	bzero(&servaddr, sizeof(servaddr));	
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(13);/* daytime server*/
	
	/*bind*/
	//绑定
	Bind(listenfd,(SA *)&servaddr,sizeof(servaddr));
	
	/*listen*/
	//进行监听
	Listen(listenfd,LISTENQ);
	//
	Signal(SIGCHLD,sig_chld);
	cout<<"In the loop..."<<endl;
	for(;;)
	{
		cout<<"Wait for connecting..."<<endl;
		
		/*accept (blocking)*/
		clilen=sizeof(cliaddr);
		connfd = Accept(listenfd,(SA *)&cliaddr, &clilen);/*read the info in the cliaddr*/
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
