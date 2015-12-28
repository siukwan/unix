/*
信号处理函数
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
	printf("child %d terminated\n",pid);
	return ;	
}