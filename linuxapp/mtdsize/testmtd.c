/*************************************************************************
	> File Name: testmtd.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Fri 01 Jun 2018 09:25:43 AM CST
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NANDCARD_NODE "/proc/mtd"
int get_system_sys_capacity(double *capacity)
{
	if (capacity == NULL)
		return 0;
	*capacity = 0;

	if(access(NANDCARD_NODE,F_OK))
		return 0;

	FILE *procpt;
	char line[256];
	double ma, mi;
	size_t size,size_total=0;

	procpt = fopen(NANDCARD_NODE, "r");
	if(procpt == NULL)
	{
		perror(NANDCARD_NODE);
		return -1;
	}
	while (fgets(line, sizeof(line), procpt))
	{
		sscanf(line,"mtd%*d: %x %*x \"%*[^\"]\"\n",&size);
		printf("%x\n",size);
		size_total+=size;
	}
	pclose(procpt);
	printf("size_total=%dM\n",size_total/1024/1024);
	return 0;
}


int main()
{
	double d;
	get_system_sys_capacity(&d);
}
