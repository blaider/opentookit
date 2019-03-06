/*************************************************************************
	> File Name: mtest.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Wed 06 Dec 2017 10:07:07 AM CST
 ************************************************************************/

#include<stdio.h>

int main()
{

#ifdef AA
	printf("AA\n");
#else
	printf("aa\n");
#endif
	return 0;
}
