/*************************************************************************
	> File Name: queuetest.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Thu 09 Mar 2017 08:22:18 PM CST
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct _daq_device_t
{
	size_t	p_start;
	size_t 	p_end;
	char	buff[20];
};
typedef struct _daq_device_t daq_device_t;

size_t buff_push(daq_device_t *daq,char *buf,int len)
{
	size_t actual_push = 0;
	size_t total = sizeof(daq->buff) -1;
	size_t left = 0;
	left = (total - daq->p_end + daq->p_start)%(total+1);
	actual_push = left>len?len:left;
	if((daq->p_end +actual_push) <= total)
	{
		memcpy(&daq->buff[daq->p_end],buf,actual_push);
		daq->p_end += actual_push;
	}else{
		size_t tail = total - daq->p_end +1;
		size_t header = actual_push - tail;
		memcpy(&daq->buff[daq->p_end],buf,tail);
		memcpy(daq->buff,&buf[tail],header);
		daq->p_end = header;
	}
	return actual_push;
}

size_t buff_pop(daq_device_t *daq,char *buf,int len)
{
	size_t actual_pop = 0;
	size_t total = sizeof(daq->buff) -1;
	size_t left = 0;
	left = (total + 1 + daq->p_end - daq->p_start)%(total+1);
	actual_pop = left>len?len:left;
	if( (daq->p_start + actual_pop) <= total)
	{
		memcpy(buf,&daq->buff[daq->p_start],actual_pop);
		daq->p_start += actual_pop;
		if(daq->p_start == (total + 1))
			daq->p_start = 0;
	}else{
		size_t tail = total - daq->p_start +1;
		size_t header = actual_pop - tail;
		memcpy(buf,&daq->buff[daq->p_start],tail);
		memcpy(&buf[tail],daq->buff,header);
		daq->p_start = header;
	}
	return actual_pop;
}

void show(daq_device_t *daq)
{
	int i=0;
	printf("[%2d,%2d]",daq->p_start,daq->p_end);
	for(i=0;i<sizeof(daq->buff);i++)
		printf("%d ",daq->buff[i]);
	printf("\n");
}
int main()
{
	char buf[10];
	int i= 10;
	for(i=0;i<sizeof(buf);i++)
		buf[i] = i+1;
	i=10;
	daq_device_t daq;
	daq.p_end = 6;
	daq.p_start = 6;
	memset(daq.buff,0,sizeof(daq.buff));
	while(i--)
	{
		buff_push(&daq,buf,10-i);
		show(&daq);
		sleep(1);
	}

	printf("pop\n");
	for(i=0;i<sizeof(daq.buff);i++)
		daq.buff[i] = i+1;
	i = 9;
	while(i--)
	{
		int nread = buff_pop(&daq,buf,10-i);
//		show(&daq);
		int j = 0;
		for(j = 0;j< nread;j++)
		{
			printf("%02d ",buf[j]);
		}
		printf("\n");
		sleep(1);
	}
}
