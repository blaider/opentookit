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
#include <signal.h>
#include <fcntl.h>

sem_t sem;
void* ComReadThread( void* arg )
{
	sem_wait(&sem);
	printf("thread print\n");
	sleep(5);
	sem_post(&sem);
	while(1)
	{
		sleep(1);
	}
	return NULL;
}
void sig_int(int no)
{
	pid_t pid = getpid();
	printf("exit %d\n",pid);
	exit(0);
	return ;
}
int main(int argc,char *argv[])
{
	int fd,fd1;
	pthread_t m_hThread;

	fd = open("pthread_main.c",O_RDONLY);
	fd1 = open("pthread_main.c",O_RDONLY);
	printf("fd:%d,fd1:%d\n",fd,fd1);
	close(fd);
	close(fd);

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
