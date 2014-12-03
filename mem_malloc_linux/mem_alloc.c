/*************************************************************************
	> File Name: mem_alloc.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Wed 03 Dec 2014 06:10:58 PM CST
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char *argv[])
{
	char * buf;
	int size = 10;
	for(;;)
	{
		buf = malloc(size*1024*1024);
		if(buf == NULL)
			break;
		free(buf);
		size += 10;
	}
	buf = malloc((size-10)*1024*1024);
	printf("%dM\n",size-10);
	memset(buf,0xa5,(size-10)*1024*1024);
	printf("Press ENTER to end\n");
	getchar();
	free(buf);
	return 0;
}
