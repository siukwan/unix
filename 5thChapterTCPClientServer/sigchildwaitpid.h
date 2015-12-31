/*
2015-12-30 waitpid信号处理函数
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

void sig_child(int signo)
{
	pid_t pid;
	int stat;
	
	pid = wait(&stat);
	while( (pid = waitpid(-1,&stat,WNOHANG))>0)
	{
		//
		//不建议在信号处理函数里面增加控制台输出	
		printf("child %d terminated\n",pid);
	}
	return ;	
}
