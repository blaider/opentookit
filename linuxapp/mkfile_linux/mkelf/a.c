/*************************************************************************
	> File Name: a.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Wed 10 Dec 2014 03:05:34 PM CST
 ************************************************************************/

#include<stdio.h>
int add(int a,int b)
{
	return a+b;
}

int main(int argc,char *argv[])
{
	printf("add:%d\n",add(10,1));
	return 0;
}
