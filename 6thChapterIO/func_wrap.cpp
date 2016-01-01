/*
包含包裹函数
*/
#include"func.h"

using namespace std;


/*包裹函数*/
//socket
int Socket(int family ,int type , int protocol)
{
	int n;
	if((n=socket(family, type, protocol)) <0)
		err_sys("socket error");
	return n;
}
//connect
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

//fputs,输出到文件描述符
void Fputs(const char *ptr, FILE *stream)
{
	if(fputs(ptr,stream)==EOF)
		err_quit("fputs error");
}
//fgets，从文件描述符中输入
char *Fgets(char *ptr,int n,FILE *stream)
{
	char *rptr = fgets(ptr, n ,stream);
	if(rptr== NULL && ferror(stream))
		err_quit("fgets error");
	return rptr;
}



//server
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

pid_t Fork(void)
{
	pid_t pid=fork();
	if(pid == -1 )
		err_quit("fork error");
	return pid;
}


//调用waitpid函数的sig_chld函数最终版本
void sig_chld(int signo)
{
	pid_t pid;
	int   stat;

	while((pid = waitpid(-1, &stat, WNOHANG))>0)
		printf("child %d terminated\n",pid);
	return ;
}

//20151230添加
Sigfunc *Signal(int signo, Sigfunc *func)
{
   Sigfunc *sigfunc = signal(signo, func);
   if ( sigfunc == SIG_ERR)
       err_quit("signal error");
   return sigfunc;
}
