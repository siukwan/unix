#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>
/* HTTP协议的方法 */
typedef enum HTTP_METHOD_TYPE {
	METHOD_GET, /*GET     方法*/
	METHOD_POST, /*POST   方法*/
	METHOD_PUT, /*PUT     方法*/
	METHOD_DELETE, /*DELETE方法*/
	METHOD_HEAD, /*HEAD   方法*/
	METHOD_NOTSUPPORT
} HTTP_METHOD_TYPE;
/*请求结构*/
struct conn_request {
	char *head; /*请求头部\0'结尾*/
	char *uri; /*请求URI,'\0'结尾*/
	int method; /*请求类型*/
	/*HTTP的版本信息*/
	unsigned long major; /*主版本*/
	unsigned long minor; /*副版本*/
	/*主机ip*/
	char *host;
	char *referer;
	char *user_ugent;
	char *accept_mime;/*接收的mime类型*/
	char *accept_language;/*语言*/
	char *accept_encodeing;/*编码方式*/
	char *accept_charset;/*字符编码*/
	char *connection;/*连接*/
};
/* 响应结构 */
struct conn_response {
	time_t birth_time; /*建立时间*/
	time_t expire_time;/*超时时间*/
	int status; /*响应状态值*/
	int cl; /*响应内容长度*/
	int method; /*请求类型*/
	int fd; /*请求文件描述符*/
	struct stat fsate; /*请求文件状态*/
};

