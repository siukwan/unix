/*This program is to check the computer's endian
 *
 */

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
//#include <sys/config.h>
using namespace std;

int main(int argc, char **argv)
{
	union
	{
		short s;
		char c[sizeof(s)];
	}un;

	un.s=0x0102;
	printf("%s: ","info about computer");
	if( sizeof(short) == 2 )
	{
		if(un.c[0]==1 && un.c[1] == 2)
			printf("big-endian\n");
		else if(un.c[0] == 2 && un.c[1] == 1 )
			printf("little-endian\n");
		else
			printf("unknown\n");
	}
	else
		printf("sizeof(short) = %d\n" , sizeof(short));
	exit(0);
}
