/*************************************************************************
	> File Name: char_array.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Fri 09 Oct 2015 01:44:28 PM CST
 ************************************************************************/

#include<stdio.h>
int main(int argc,char *argv[])
{
	char *errlist[]
		=
		{
			[0]="none",
			[1]="first",
			[3]="second",
		};
	printf("%s\n",errlist[0]);
	printf("%s\n",errlist[1]);
	printf("%s\n",errlist[3]);
	printf("%s\n",errlist[2]);

	return 0;
}
