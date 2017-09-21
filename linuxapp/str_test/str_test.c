/*************************************************************************
	> File Name: str_test.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Wed 28 Dec 2016 11:00:40 AM CST
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>

#pragma pack(1)
struct aa
{
	char a;
	char b;
	char *p;
};
#pragma pack()

typedef struct aa SA;

int main(int argc,char *argv[])
{
	char p[] = {10,5,'a','b',0};
	SA *pa = (SA *)p;
	printf("%d\n",pa->a);
	printf("%d\n",pa->b);
	printf("%s\n",(char *)&pa->p);
	printf("%p,%p,%p,%p\n",pa,&pa->a,&pa->b,pa->p);
	return 0;
}
