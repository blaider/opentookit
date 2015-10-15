/*************************************************************************
 > File Name: getopt.c
 > Author: suchao.wang
 > Mail: suchao.wang@advantech.com.cn
 > Created Time: Wed 03 Dec 2014 07:42:29 PM CST
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>
#include <errno.h>
#include<unistd.h>

int main(int argc, char **argv)

{

	const char *p1;
	char * const p2 = p1;
	char *p3;


	printf("%p,%x,%p,%x,%p,%x\n",p1,*p1,p2,*p2,p3,*p3);
	p1 = p2;
	printf("%p,%x,%p,%x,%p,%x\n",p1,*p1,p2,*p2,p3,*p3);
	*p2 = 2;
	printf("%p,%x,%p,%x,%p,%x\n",p1,*p1,p2,*p2,p3,*p3);

	return 0;
}
