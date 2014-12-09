/*************************************************************************
	> File Name: time_main.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Fri 28 Nov 2014 03:57:02 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

sem_t sem;
void* ComReadThread( void* arg )
{
	sem_wait(&sem);
	printf("thread print\n");
	sleep(5);
	sem_post(&sem);
	return NULL;
}
void sig_int(int intno)
{
	printf("i will exit1\n");
	printf("i will exit2\n");
	printf("i will exit3\n");
	printf("i will exit4\n");
	printf("i will exit5\n");
	exit(0);
}
int main(int argc,char *argv[])
{

	pthread_t m_hThread;

	signal(SIGINT,sig_int);

	if (sem_init(&sem, 0, 0) == -1)
			return -1;
	int error = pthread_create(&m_hThread, NULL, ComReadThread,NULL);
	if(error == -1 )
	{
		perror("create pthread");
	}
	sleep(5);
	sem_post(&sem);
	pthread_join(m_hThread,NULL);
	return 0;
}
