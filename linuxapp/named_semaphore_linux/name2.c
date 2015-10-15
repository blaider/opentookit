/*************************************************************************
	> File Name: time_main.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Fri 28 Nov 2014 03:57:02 PM CST
 ************************************************************************/
//#getconf GNU_LIBPTHREAD_VERSION
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <memory.h>
#include <stdbool.h>

#define I2CMUTEX "I2CMUTEX"
sem_t *m_pSem;
void sig_int(int no)
{
	pid_t pid = getpid();
	printf("exit %d\n",pid);
	exit(0);
	return ;
}
int main(int argc,char *argv[])
{

	m_pSem = sem_open(I2CMUTEX, O_RDWR | O_CREAT, 0644, 1);
	printf("sem:%x\n",m_pSem);
	if (NULL == m_pSem)
	{
		perror("sem_open");
		return false;
	}
	sleep(10);
	int ret = sem_close(m_pSem);
	if (0 != ret)
	{
		perror("sem_close");
		//		printf("sem_close error %d\n", ret);

	}
	//	sem_unlink(I2CMUTEX);
	m_pSem = NULL;
	return 0;
}
