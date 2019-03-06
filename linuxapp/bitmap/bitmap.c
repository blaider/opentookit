/*************************************************************************
	> File Name: bitmap.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Sat 03 Mar 2018 09:50:29 AM CST
 ************************************************************************/

#include<stdio.h>



struct bm{
	char b:4;
	char c:4;
	char d:4;
	char e:4;
};

union ua{
	short a;
	struct bm bm;
};

int main()
{
	union ua uaa;
	uaa.a = 0;
	uaa.bm.b= 1;
	printf("%x\n",uaa.a);
	uaa.bm.c = 2;
	printf("%x\n",uaa.a);
	uaa.bm.d = 3;
	printf("%x\n",uaa.a);
	return 0;
}
