/*************************************************************************
	> File Name: ma.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Thu 17 Dec 2015 10:13:07 AM CST
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include <string.h>
#include <pthread.h>


pthread_mutex_t mutex_x= PTHREAD_MUTEX_INITIALIZER;

#define MAXARR 10
#define NODESIZE 32

static char *pindex[MAXARR];
static int 	ind = 0;

void initarray()
{
	int i=0;
	pindex[0] = (char*)malloc(MAXARR*NODESIZE);
	for(i=1;i<MAXARR;i++)
		pindex[i] = pindex[i-1]+NODESIZE;

	for(i=0;i<MAXARR;i++)
		printf("%d=%p\n",i,pindex[i]);
	return;
}

char * ma()
{
	char *p=NULL;
	pthread_mutex_lock(&mutex_x);
	if((ind + 1) < MAXARR)
	{
		p = pindex[ind++];
	}
	pthread_mutex_unlock(&mutex_x);
	return p;
}

void fa(char *p)
{
	pthread_mutex_lock(&mutex_x);
	printf("%s:%d,%p\n",__func__,ind,p);
	pindex[--ind] = p;
	pthread_mutex_unlock(&mutex_x);
	return;
}


void *client(void *data)
{
	int pid =(int ) data;
	char *p = NULL;
	int s = 0;
	s = 1+(int)(10.0*rand()/(RAND_MAX+1.0));
	printf("%02d,s1:%d\n",pid,s);
	sleep(s);
	p = ma();
	if(p != NULL)
	{
		s = 1+(int)(10.0*rand()/(RAND_MAX+1.0));
		printf("%02d,s2:%d,%p\n",pid,s,p);
		sleep(s);
		fa(p);
	}
	return NULL;

}
int main(void) {

    int i=1;
    pthread_t threadInfo[10];
    pthread_attr_t threadInfo_attr;

    initarray();

    for(i=0;i<10;i++)
    	pthread_create(&threadInfo[i],NULL,client,(void *)i);

    for(i=0;i<10;i++)
    	pthread_join(threadInfo[i],NULL);

    for(i=0;i<MAXARR;i++)
		printf("%d=%p\n",i,pindex[i]);

    for(i=0;i<10;i++)
		pthread_create(&threadInfo[i],NULL,client,(void *)i);

	for(i=0;i<10;i++)
		pthread_join(threadInfo[i],NULL);

	for(i=0;i<MAXARR;i++)
		printf("%d=%p\n",i,pindex[i]);
    //fprintf(stdout,"Hi");
    return EXIT_SUCCESS;
}
