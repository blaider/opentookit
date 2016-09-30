/*************************************************************************
	> File Name: uniotest.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Thu 30 Jun 2016 11:35:20 AM CST
 ************************************************************************/

#include<stdio.h>
int main(int argc ,char *argv[])
{

	union aa{
		char data;
		struct {
char bOne :1;
char bTwo :1;
char bThree :2;
		};
	}a;
	a.data = 0;
	printf("%d\n",a.data);
	a.bOne = 1;
	printf("%d\n",a.data);
	a.bOne = 0;
	a.bTwo = 1;
	printf("%d\n",a.data);
	a.data = 255;
	printf("%x\n",a.bThree);
	a.data = 0;
	a.bThree = 3;
	printf("%x\n",a.data);
	printf("%x\n",a.bThree);
	a.bThree = 0x1;
	printf("%x\n",a.data);
	printf("%x\n",a.bThree);
	union aa b;
	b.data = 0;
	b.bThree = 0x1;
	printf("%x\n",b.data);
	printf("%x\n",b.bThree);
	return 0;
}
