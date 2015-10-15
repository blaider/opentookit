/*************************************************************************
	> File Name: lock.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Mon 16 Feb 2015 05:33:29 PM CST
 ************************************************************************/

#include<stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <memory.h>
#include <stdbool.h>


int Lockfile(const int iFd)
{
	struct flock    stLock;
	int i=10;
	int ret=-1;

	stLock.l_type = F_WRLCK;        /* F_RDLCK, F_WRLCK, F_UNLCK */
	stLock.l_start = 0;    /* byte offset, relative to l_whence */
	stLock.l_whence = SEEK_SET;    /* SEEK_SET, SEEK_CUR, SEEK_END */
	stLock.l_len = 0;        /* #bytes (0 means to EOF) */

	printf("will lock:%d\n",iFd);
	do
	{
		ret = (fcntl(iFd, F_SETLK, &stLock));
		printf("ret:%d\n",ret);
		if(ret == -1 && i>0)
		{
			i--;
			perror("lock file");
			usleep(100000);
			continue;
		}
		break;
	}while(1);
	printf("end lock:%d\n",iFd);
	return ret;
}
int UnLockfile(const int iFd)
{
	struct flock    stLock;

	stLock.l_type = F_UNLCK;        /* F_RDLCK, F_WRLCK, F_UNLCK */
	stLock.l_start = 0;    /* byte offset, relative to l_whence */
	stLock.l_whence = SEEK_SET;    /* SEEK_SET, SEEK_CUR, SEEK_END */
	stLock.l_len = 0;        /* #bytes (0 means to EOF) */

	return (fcntl(iFd, F_SETLK, &stLock));
}

static int lockfd = -1;
static pthread_mutex_t mutex_i2c0= PTHREAD_MUTEX_INITIALIZER;

int i2c_mutex_init()
{
	printf("%s:+++\n",__func__);
	pthread_mutex_lock(&mutex_i2c0);
	lockfd = open("/tmp/i2c0", O_RDWR|O_CREAT);
	if (lockfd < 0)
	{
		pthread_mutex_unlock(&mutex_i2c0);
		printf("%s:***\n",__func__);
		return 0;
	}
	pthread_mutex_unlock(&mutex_i2c0);
	printf("%s:---\n",__func__);
	return true;
}
int i2c_mutex_lock()
{
	printf("%s:+++\n",__func__);
	int ret=false;
	pthread_mutex_lock(&mutex_i2c0);
	if(lockfd <0)
	{
		pthread_mutex_unlock(&mutex_i2c0);
		printf("%s:***\n",__func__);
		return 0;
	}
	ret = Lockfile(lockfd);
	pthread_mutex_unlock(&mutex_i2c0);
	printf("%s:---\n",__func__);
	return !ret;
}
int i2c_mutex_unlock()
{
	printf("%s:+++\n",__func__);
	int ret=0;
	pthread_mutex_lock(&mutex_i2c0);
	if(lockfd <0)
	{
		pthread_mutex_unlock(&mutex_i2c0);
		printf("%s:***\n",__func__);
		return 0;
	}
	ret = UnLockfile(lockfd);
	pthread_mutex_unlock(&mutex_i2c0);
	printf("%s:---\n",__func__);
	return !ret;
}
int i2c_mutex_uninit()
{
	printf("%s:+++\n",__func__);
	int ret=0;
	pthread_mutex_lock(&mutex_i2c0);
	if(lockfd <0)
	{
		pthread_mutex_unlock(&mutex_i2c0);
		printf("%s:***\n",__func__);
		return 0;
	}
	ret = close(lockfd);
	pthread_mutex_unlock(&mutex_i2c0);
	printf("%s:---\n",__func__);
	return !ret;
}
int test()
{
	int i=10;
	i2c_mutex_init();
	if(!i2c_mutex_lock())
	{
		i2c_mutex_uninit();
		return 0;
	}
	while(i--)
	{
		printf("%d\n",i);
		sleep(1);
	}
	i2c_mutex_unlock();
	i2c_mutex_uninit();
	return 1;
}
int main()
{
	while(!test())
		sleep(1);

}
