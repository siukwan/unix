#include <time.h>
#include <stdio.h>
#include <string.h>
#include "get_time.h"

char *get_time_str(char *time_buf) {
	time_t now_sec;
	struct tm *time_now;
	if (time(&now_sec) == -1) {
		perror("time() in get_time.c");
		return NULL;
	}
	if ((time_now = gmtime(&now_sec)) == NULL) {
		perror("localtime in get_time.c");
		return NULL;
	}
	char *str_ptr = NULL;
	if ((str_ptr = asctime(time_now)) == NULL) {
		perror("asctime in get_time.c");
		return NULL;
	}
	strcat(time_buf, str_ptr);
	return time_buf;
}
