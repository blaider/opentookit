/*************************************************************************
	> File Name: pingtest.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Sun 01 Feb 2015 07:31:07 PM CST
 ************************************************************************/

#include<stdio.h>
#include "log.h"
int main(int argc,char *argv[])
{

	while(1)
	{
		selfping(argv[1]);
		usleep(10);
	}
	return 0;
}
