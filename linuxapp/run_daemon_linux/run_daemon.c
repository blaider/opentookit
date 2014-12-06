/*************************************************************************
	> File Name: run_daemon.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Fri 05 Dec 2014 06:59:17 PM CST
 ************************************************************************/

#include<stdio.h>
#include <errno.h>

int main(int argc, char *argv)
{
	pid_t pid;
	pid = fork();
	if (pid == 0)
	{/* 子进程执行此命令 */
		setsid();
		pid = fork();
		if (pid == 0)
		{
			execv("ls", argv);
			/* 如果exec函数返回，表明没有正常执行命令，打印错误信息*/
			LOGEX(_T("start deamon process %s failed"), "ls");
			exit(errno);
		}
		exit(0);
	}
	return 0;
}
