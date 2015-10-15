/*************************************************************************
	> File Name: sharemem.cpp
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Wed 29 Apr 2015 05:27:12 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#include "sharemem.h"

#define __DEBUG
#ifdef __DEBUG
#define DBG(fmt,args...) fprintf(stdout,  fmt,  ##args)
#else
#define DBG(fmt,args...)
#endif
#define ERR(fmt,args...) fprintf(stderr,  fmt,  ##args)


#define MEM_KEY  0x1111
/*共享内存大小*/
#define MEM_SIZE 1024

static int g_mid;
/*共享内存初始化*/
int ShareMemInit(PSHRINFO * info)
{
	int needInit = 0;
	g_mid = shmget(MEM_KEY,MEM_SIZE,IPC_CREAT | 0660);
	if(g_mid < 0 ){
		g_mid = shmget(MEM_KEY, 0, 0);
	}else{
		needInit = 1;
	}
	DBG("shared memory id:%d\n",g_mid);
	if(g_mid < 0 )
		return -1;

	*info = (PSHRINFO)shmat(g_mid,0,0);
	if(*info == NULL)
		return -2;
	if(needInit)
	{
		memset(*info,0,sizeof(SHRINFO));
		sem_init(&(*info)->sem_rw,0,1);
	}
	return 0;
}
/*从共享内存中指定的偏移量处读取数据*/
void ShareMemRead(int offset,void *buf,int length)
{
	char *pSrc = (char *)shmat(g_mid,0,0);
	if(pSrc == NULL)
		return;
	memcpy(buf,pSrc+offset,length);
	shmdt(pSrc);
}
/*从共享内存中指定的偏移量处写数据*/
void ShareMemWrite(int offset,void *buf,int length)
{
	char *pDst = (char *)shmat(g_mid,0,0);
	if(pDst == NULL)
		return;
	memcpy(pDst+offset,buf,length);
	shmdt(pDst);
}


