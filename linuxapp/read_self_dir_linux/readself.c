/*************************************************************************
	> File Name: readself.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Fri 05 Dec 2014 06:51:33 PM CST
 ************************************************************************/

#include<stdio.h>
#include <limits.h>
#include <unistd.h>

int main(int argc,char *argv[])
{
		char szModule[PATH_MAX] = {0};
		int rslt = readlink("/proc/self/exe", szModule, sizeof(szModule) - 1);
		printf("szModule:%s\n",szModule);
		szModule[rslt] = '\0';
		for (int i = rslt; i >= 0; i--)
		{
			if (szModule[i] == '/')
			{
				szModule[i + 1] = '\0';
				break;
			}
		}
		printf("szModule:%s\n",szModule);
		return 0;
}
