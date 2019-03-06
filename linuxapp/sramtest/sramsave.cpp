/*************************************************************************
	> File Name: sramsave.cpp
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Tue 07 Nov 2017 11:48:45 AM CST
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include<iostream>
using namespace std;


#pragma pack(1)
typedef  struct SRAM_CTX
{
	uint16_t tagcrc;
	/** Tag value */
	double value;

	/** Tag time stamp, in UTC format */
	uint64_t time;

	/** Tag time stamp, microsecond part */
	uint32_t usec;

	char rev[10];

}SRAM_CTX;
typedef struct SRAM_HDR
{
	char magic[4];
	uint32_t len;
	uint16_t point;
	uint16_t flag;
	char rev[20];
}SRAM_HDR;
#pragma pack()

#define SRAM_DATA_LEN (sizeof(SRAM_CTX))

#define SRAM_NUMS (SRAM_SIZE/sizeof(SRAM_CTX))

int sram_fd = -1;

#define SRAM_DEV 		"/dev/sram"
#define SRAM_BASE_ADDR 0 //(8*1024)
#define SRAM_SIZE (32*1024)

int loadSramData(size_t offset,char *data,int len)
{
	if(sram_fd == -1 || data == NULL || len <= 0 || offset <0 || offset > (SRAM_NUMS -1))
		return -1;
	int ret = 0,count = 0;
//	printf("%s:%d,%d\n",__func__,offset,len);
	size_t actOffset=SRAM_BASE_ADDR+SRAM_DATA_LEN*offset;
	if ((ret = lseek(sram_fd, actOffset, SEEK_SET)) < 0)
	{
		printf("\nError to perform lseek: ret = %d", ret);
		goto err;
	}

//	printf("%s:%d\n",__func__,actOffset);
	lseek(sram_fd, actOffset, SEEK_SET);

	count = read(sram_fd, NULL, len);
	if (count != len)
	{
		printf("\nError to write test %d\n", count);
		goto err;
	}
	return 0;
err:

	return -2;
}

int saveSramData(size_t offset,char *data,int len)
{
	if(data == NULL || len <= 0 || offset <0 || offset > (SRAM_NUMS -1) || sram_fd == -1)
		return -1;
//	printf("%s:%d,%d\n",__func__,offset,len);
	int ret = 0,count = 0;
	size_t actOffset=SRAM_BASE_ADDR+SRAM_DATA_LEN*offset;
	if ((ret = lseek(sram_fd, actOffset, SEEK_SET)) < 0)
	{
		printf("\nError to perform lseek: ret = %d", ret);
		goto err;
	}
//	printf("%s:%d\n",__func__,actOffset);
	lseek(sram_fd, actOffset, SEEK_SET);

	count = write(sram_fd, data+1, len);
	if (count != len)
	{
		printf("\nError to write test %d\n", count);
		goto err;
	}
//	if(len == sizeof(SRAM_CTX))
//	{
//		SRAM_CTX *pctx = (SRAM_CTX *)data;
////		printf("%s:crc=%x\n",__func__,pctx->tagcrc);
//		printf("%s:value=%lf\n",__func__,pctx->value);
//	}
	return 0;
err:

	return -2;
}


extern "C" int main ( int argc, char* argv[] )
{
	int i=0,j=0;
	printf("SRAM_NUMS:%d\n",SRAM_NUMS);
	sram_fd = open(SRAM_DEV, O_RDWR);
	if( sram_fd == -1)
		return -1;

	SRAM_CTX sran[2];
	for(;;)
	{
		printf("%d======\n",i);
		memset(&sran,i,sizeof(sran));
		for(j=0;j<SRAM_NUMS;j++)
		{
			saveSramData(j,(char *)&sran[0],sizeof(sran[0]));
			loadSramData(j,(char *)&sran[0],sizeof(sran[0]));
		}
		printf("%d----\n",i++);
	}
}
