/*************************************************************************
	> File Name: getuser.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Fri 05 Dec 2014 07:22:52 PM CST
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc,char*argv[])
{
	printf("getuid():%d\n",getuid());
	printf("geteuid():%d\n",geteuid());
	if (geteuid() != 0) {
	     fprintf(stderr, "This program must run as root\n");
	     exit(1);
	}
	return 0;
}
