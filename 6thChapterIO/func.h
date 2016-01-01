/*
2016-01-01 包裹函数头文件
*/
#ifndef MY_FUNC_H_
#define MY_FUNC_H_
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
//wait和waitpid函数都在wait.h里面
#include<sys/wait.h>
//select函数在select.h中
//#include<sys/select.h>

#define SA sockaddr
#define MAXLINE 4096
#define LISTENQ 1024
#define bzero(ptr,n) memset(ptr,0,n)
#define recv(a,b,c,d) recvfrom(a,b,c,d,0,0)
#define send(a,b,c,d) sendto(a,b,c,d,0,0)
#define MYPORT 9877

//20151231
typedef void Sigfunc(int);
//20160101
struct args{
	long arg1;
	long arg2;
};
struct result{
	long sum;
};
//client
extern void err_doit(int errnoflag, int level ,const char *fmt, va_list ap);
extern void err_sys(const char *fmt,...);
extern void err_quit(const char *fmt,...);
extern int  Socket(int family ,int type , int protocol);
extern void Connect(int fd, const struct sockaddr *sa, socklen_t salen);
extern void Writen(int fd, void *ptr, size_t nbytes);
extern void Fputs(const char *ptr, FILE *stream);
extern char *Fgets(char *ptr,int n,FILE *stream);

//server
extern void     Bind(int fd, const struct sockaddr *sa, socklen_t salen);
extern void     Listen(int fd, int backlog);
extern int      Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);
extern void     Write(int fd, void *ptr, size_t nbytes);
extern void     Close(int fd);
extern pid_t    Fork(void);
extern void     sig_chld(int signo);
extern Sigfunc *Signal(int signo, Sigfunc *func);

extern int  Select(int nfds, fd_set *readfds, fd_set *writefds,fd_set *exceptfds, struct timeval *timeout);
//select01
extern void str_cli_select01(FILE *fp,int sockfd);

#endif
