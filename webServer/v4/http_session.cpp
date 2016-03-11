#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <ctype.h>
#include "http_session.h"
#include "get_time.h"
#include "http_protocol.h"


int http_session(int *connect_fd, struct sockaddr_in *client_addr) {
	char recv_buf[RECV_BUFFER_SIZE + 1];/*recv_buf保存接收浏览器发送的信息*/
	unsigned char send_buf[SEND_BUFFER_SIZE + 1];/*send_buf保存接发送给收浏览器的信息*/
	unsigned char file_buf[FILE_MAX_SIZE + 1];/*file_buf浏览器请求的文件*/
	/*memset(void *s ,int c , size_t n)会将参数S所指的内存区域前n个字节以c填入，后返回指针S
	 * 参数c虽然声明为int ， 但必须为unsidned char ，所以范围在0-255
	 * 功能：初始化数组
	 */
	memset(recv_buf, '\0', sizeof(recv_buf));
	memset(send_buf, '\0', sizeof(send_buf));
	memset(file_buf, '\0', sizeof(file_buf));

	char uri_buf[URI_SIZE + 1];/*浏览器请求的URI*/
	memset(uri_buf, '\0', sizeof(uri_buf));

	int maxfd = *connect_fd + 1;/*取得文件描述符加1后的值，在select（）需要使用*/
	fd_set read_set;
	FD_ZERO(&read_set);/*清空描述符列表*/

	struct timeval timeout;
	timeout.tv_sec = TIME_OUT_SEC;
	timeout.tv_usec = TIME_OUT_USEC;

	int res = 0;
	int read_bytes = 0;/*保存接收数据的大小*/
	int send_bytes = 0;/*保存发送数据的大小*/
	int file_size = 0;/*保存请求文件的大小*/
	char *mime_type;
	int uri_status;
	FD_SET(*connect_fd, &read_set);
	char req_method[1024];
	struct conn_request conn_req;
	int msg = 200;
	while (1) {
		/*select（）参数第一个参数代表最大的文件描述词加1
		 * timeout等待时间
		 * */
		res = select(maxfd, &read_set, NULL, NULL, &timeout);/*多路复用*/
		if(res==-1){/*错误处理*/
			perror("select() error. in http_sesseion.c");
			close(*connect_fd);
			return -1;
		}else if(res == 0){/*超时处理*/
			continue;
			break;
		}

		if (FD_ISSET(*connect_fd, &read_set)) {
			memset(recv_buf, '\0', sizeof(recv_buf));
			if ((read_bytes = recv(*connect_fd, recv_buf, RECV_BUFFER_SIZE, 0))== 0) {
				return 0;
			} else if (read_bytes > 0){
				conn_req.head = recv_buf;

				if (is_http_protocol(conn_req.head) == 0){ /* 判断是否为HTTP protocol */
					fprintf(stderr, "Not http protocol.\n");
					close(*connect_fd);
					return -1;
				} else{ /* http 协议  */
					char *edition = get_http_edition(conn_req.head);/*获取http协议版本号*/
					conn_req.major = edition[5]-48;
					conn_req.minor = edition[7]-48;

					memset(uri_buf, '\0', sizeof(uri_buf));
					if (get_uri(conn_req.head, uri_buf) == NULL){ /* 获取请求的URI */
						uri_status = URI_TOO_LONG;
					} else {
						conn_req.uri = uri_buf;
						printf("URI:%s\n", conn_req.uri);

						memset(req_method, '\0', sizeof(req_method));
						conn_req.method = get_request_method(recv_buf , req_method);/*获取method*/
						uri_status = get_uri_status(conn_req.uri);
						switch (uri_status) {
						case FILE_OK:
							printf("file ok\n");
							break;
						case FILE_NOT_FOUND: /* file 不存在*/
							printf("in switch on case FILE_NOT_FOUND\n");
							conn_req.uri = "404.html";
							msg =404;
							break;
						case FILE_FORBIDEN: /* url禁止执行访问  */
							break;
						case URI_TOO_LONG: /* 请求 uri 太长 */
							break;
						default:
							break;
						}
						mime_type = get_mime_type(conn_req.uri);/*获取数据类型信息*/
						conn_req.accept_mime = mime_type;
						printf("mime type: %s\n", conn_req.accept_mime);
						file_size = get_file_disk(conn_req.uri, file_buf);/*从服务器指定的目录获取uri中的文件*/
						/*将客户访问的uri路径文件与浏览器的头部消息组装*/
						send_bytes = reply_normal_information(send_buf,file_buf, file_size, conn_req.accept_mime , msg);
						send(*connect_fd, send_buf, send_bytes, 0);
					}
				}
			}
		}
	}
	return 0;
}

/**
 * 判断是否为http请求
 */
int is_http_protocol(char *msg_from_client) {

	char *http = strstr(msg_from_client , "HTTP/" );
	if(http[6]=='.'){
		return 1;
	}
	return 0;
}

char* get_http_edition(char *msg_from_client) {

	char *http = strstr(msg_from_client , "HTTP/" );

	if(http[6]=='.'){
		return http;
	}
	return NULL;
}

int get_request_method(char *req_header , char *req_method) {
	int index = 0;
	while(req_header[index]!=' '){
		index++;
	}
	strncpy(req_method, req_header , index);
	if(strncmp("GET", req_method, 3)){
		return METHOD_GET;
	}else if(strncmp("POST", req_method, 4)){
		return METHOD_POST;
	}else if(strncmp("PUT", req_method, 3)){
		return METHOD_PUT;
	}else if(strncmp("DELETE", req_method, 6)){
		return METHOD_DELETE;
	}else if(strncmp("HEAD", req_method, 4)){
		return METHOD_HEAD;
	}else if(strncmp("NOTSUPPORT", req_method, 10)){
		return METHOD_NOTSUPPORT;
	}
	return 0;
}

char *get_uri(char *req_header, char *uri_buf) {
	int index = 0;
	while ((req_header[index] != '/') && (req_header[index] != '\0')) {
		index++;
	}
	int base = index;
	while (((index - base) < URI_SIZE) && (req_header[index] != ' ')
			&& (req_header[index] != '\0')) {
		index++;
	}
	if ((index - base) >= URI_SIZE) {
		fprintf(stderr, "error: too long of uri request.\n");
		return NULL;
	}
	if ((req_header[index - 1] == '/') && (req_header[index] == ' ')) {
		strcpy(uri_buf, "index.html");
		return uri_buf;
	}
	strncpy(uri_buf, req_header + base + 1, index - base - 1);
	return uri_buf;

}

int get_uri_status(char *uri) {
	/**access()检查是否可以读写某个已存在的文件 F_ok判断文件是否存在，R_OK是否可以读取*/
	if (access(uri, F_OK) == -1) {
		fprintf(stderr, "File: %s 找不到.\n", uri);
		return FILE_NOT_FOUND;
	}
	if (access(uri, R_OK) == -1) {
		fprintf(stderr, "File: %s 不能读取.\n", uri);
		return FILE_FORBIDEN;
	}
	return FILE_OK;
}

char *get_mime_type(char *uri) {
	int len = strlen(uri);
	int dot = len - 1;
	while (dot >= 0 && uri[dot] != '.') {
		dot--;
	}
	if (dot == 0){ /* if the uri begain with a dot and the dot is the last one, then it is a bad uri request,so return NULL  */
		return NULL;
	}
	if (dot < 0){ /* the uri is '/',so default type text/html returns */
		return "text/html";
	}
	dot++;
	int type_len = len - dot;
	char *type_off = uri + dot;
	switch (type_len) {
	case 4:
		if (!strcmp(type_off, "html") || !strcmp(type_off, "HTML")) {
			return "text/html";
		}
		if (!strcmp(type_off, "jpeg") || !strcmp(type_off, "JPEG")) {
			return "image/jpeg";
		}
		break;
	case 3:
		if (!strcmp(type_off, "htm") || !strcmp(type_off, "HTM")) {
			return "text/html";
		}
		if (!strcmp(type_off, "css") || !strcmp(type_off, "CSS")) {
			return "text/css";
		}
		if (!strcmp(type_off, "png") || !strcmp(type_off, "PNG")) {
			return "image/png";
		}
		if (!strcmp(type_off, "jpg") || !strcmp(type_off, "JPG")) {
			return "image/jpeg";
		}
		if (!strcmp(type_off, "gif") || !strcmp(type_off, "GIF")) {
			return "image/gif";
		}
		if (!strcmp(type_off, "txt") || !strcmp(type_off, "TXT")) {
			return "text/plain";
		}
		break;
	case 2:
		if (!strcmp(type_off, "js") || !strcmp(type_off, "JS")) {
			return "text/javascript";
		}
		break;
	default: /* unknown mime type or server do not support type now*/
		return "NULL";
		break;
	}

	return NULL;
}

int get_file_disk(char *uri, unsigned char *file_buf) {
	int read_count = 0;
	int fd = open(uri, O_RDONLY);
	if (fd == -1) {
		perror("open() in get_file_disk http_session.c");
		return -1;
	}
	unsigned long st_size;
	struct stat st;
	/**fstat(int fd , struts stat *st)将用来将参数fd*所指的文件状态，复制到参数stat所指的结构中，fd所指的文件必须先打开*/
	if (fstat(fd, &st) == -1) {
		perror("stat() in get_file_disk http_session.c");
		return -1;
	}
	st_size = st.st_size;
	if (st_size > FILE_MAX_SIZE) {
		fprintf(stderr, "the file %s is too large.\n", uri);
		return -1;
	}
	if ((read_count = read(fd, file_buf, FILE_MAX_SIZE)) == -1) {
		perror("read() in get_file_disk http_session.c");
		return -1;
	}
	printf("file %s size : %lu , read %d.\n", uri, st_size, read_count);
	return read_count;
}

int reply_normal_information(unsigned char *send_buf, unsigned char *file_buf,
		int file_size, char *mime_type , int msg) {
//	char *str="HTTP/1.1 %d OK\r\nServer:Mutu/Linux(0.1)\r\nDate:" ;
	char *str="HTTP/1.1 " ;
	register int index = strlen(str);
	/*memcpy（void *dest ，const void *src ， size_t n）
	 * 用来拷贝src所指内存的内容前N个字符到dest所在的地址上
	 * 不会因为‘\0’而结束
	 * */
	memcpy(send_buf, str, index);
	char msg_len[8];
	memset(msg_len, '\0', sizeof(msg_len));
	sprintf(msg_len, "%d", msg);
	int len = strlen(msg_len);
	memcpy(send_buf + index,msg_len, len);
	index += len;

	str=" OK\r\nServer:Mutu/Linux(0.1)\r\nDate:" ;
	len = strlen(str);
	memcpy(send_buf + index,str, len);
	index += len;

	char time_buf[TIME_BUFFER_SIZE];
	memset(time_buf, '\0', sizeof(time_buf));
	str = get_time_str(time_buf);
	len = strlen(time_buf);
	memcpy(send_buf + index, time_buf, len);
	index += len;

	len = strlen(ALLOW);
	memcpy(send_buf + index, ALLOW, len);
	index += len;

	memcpy(send_buf + index, "\r\nContent-Type:", 15);
	index += 15;
	len = strlen(mime_type);
	memcpy(send_buf + index, mime_type, len);
	index += strlen(mime_type);

	memcpy(send_buf + index, "\r\nContent-Length:", 17);
	index += 17;
	char num_len[8];
	memset(num_len, '\0', sizeof(num_len));
	sprintf(num_len, "%d", file_size);
	len = strlen(num_len);
	memcpy(send_buf + index, num_len, len);
	index += len;

	memcpy(send_buf + index, "\r\n\r\n", 4);
	index += 4;

	memcpy(send_buf + index, file_buf, file_size);
	index += file_size;
	return index;

}


