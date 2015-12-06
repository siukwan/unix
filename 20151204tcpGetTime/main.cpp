#include<stdio.h>
#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#define MAXLINE 4096
using namespace std;

void err_quit(string str)
{
cout<<str<<endl;
//exit(0);
}
void err_sys(string str)
{
cout<<str<<endl;
//exit(0);
}

int main(int argc,char **argv)
{
    int sockfd,n;
    char recvline[MAXLINE+1];
    sockaddr_in servaddr;//the sockaddr_in defination is in the headfile <netinet/in.h>

    if(argc !=2)
        err_quit("usage: a.put <IPaddress>");

    if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
        err_sys("socket error");
    bzero(&servaddr,sizeof(servaddr));
    cout<<"test"<<endl;
    printf("123\n");
    return 0;
}
~                                                                                                                                 
~        
