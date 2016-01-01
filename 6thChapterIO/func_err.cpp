/*
包含错误函数：err
*/
#include<func.h>
using namespace std;
#define MAXLINE 4096
int daemon_proc;

//记录日志
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

//系统错误
void err_sys(const char *fmt,...)
{
	va_list ap;
	va_start(ap,fmt);
	err_doit(1,LOG_ERR,fmt,ap);
	va_end(ap);
	exit(1);
}

//错误退出，和上面一样
void err_quit(const char *fmt,...)
{
	va_list ap;
	va_start(ap,fmt);
	err_doit(0,LOG_ERR, fmt,ap);
	va_end(ap);
	exit(1);
}
