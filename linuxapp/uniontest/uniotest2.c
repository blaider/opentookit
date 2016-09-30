/*************************************************************************
	> File Name: uniotest2.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Thu 30 Jun 2016 01:15:43 PM CST
 ************************************************************************/

#include<stdio.h>
int main()
{
	struct {
		union{
			char data;
			struct{
				char b1:1;
				char b2:2;
				char b3:3;
				char b4:2;
			};
		};
	}aa;
	aa.data = 0;
	printf("%d\n",aa.data);
	aa.data = 0;
	aa.b1=1;
	printf("%d\n",aa.data);
	aa.data = 0;
	aa.b2=1;
	printf("%d\n",aa.data);
	aa.data = 0;
	aa.b3=1;
	printf("%d\n",aa.data);
	aa.data = 0;
	aa.b4=1;
	printf("%d\n",aa.data);
}
