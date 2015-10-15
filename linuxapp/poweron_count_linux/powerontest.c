/*************************************************************************
	> File Name: powerontest.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Thu 05 Mar 2015 11:01:51 AM CST
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define COUNTFILE "/etc/poweron"
int main(int argc,char *argv[])
{
	FILE *file;
	int poweron=0;
	char buf[256];

	if(!access(COUNTFILE,F_OK))
	{
		file =fopen(COUNTFILE,"r");
		memset(buf,0,sizeof(buf));
		fgets(buf,sizeof(buf),file);
		sscanf(buf, "%d", &poweron);
		fclose(file);
	}
	file=fopen(COUNTFILE,"w+");
	sprintf(buf,"%d",++poweron);
	fwrite(buf,strlen(buf),1,file);
	fclose(file);
	sync();
	printf("PowerOn:%4d\n",poweron);
	return 0;
}

