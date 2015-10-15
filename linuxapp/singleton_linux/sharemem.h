/*************************************************************************
	> File Name: sharemem.h
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Wed 29 Apr 2015 05:27:23 PM CST
 ************************************************************************/

#ifndef _SHARE_MEM_H_
#define _SHARE_MEM_H_

#include <stdint.h>
#include <time.h>
#include <semaphore.h>
#include "status.h"

#define MAX_PROCESS_TIME (10*60)

typedef struct _shareInfo
{
	sem_t 	sem_rw;
	STATUS status;
}SHRINFO,*PSHRINFO;

int ShareMemInit(PSHRINFO * info);
void ShareMemRead(int offset, void *buf, int length);
void ShareMemWrite(int offset, void *buf, int length);

#endif


