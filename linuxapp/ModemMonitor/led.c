/*************************************************************************
	> File Name: led.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Mon 05 Nov 2018 03:08:00 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/mman.h>

#include "led.h"
#include "Log.h"

static pthread_mutex_t led_mutex =PTHREAD_MUTEX_INITIALIZER;
static int led_status = 0;


#define GPIO1_BASE 0x4804C000

static void *viraddr = NULL;
static int mapfd = -1;

/*
void static gpio_set(void * addr,unsigned int offset)
{
	 *(volatile unsigned int *) addr |= 1 << offset;
	 return;
}
void static gpio_clear(void * addr,unsigned int offset)
{
	 *(volatile unsigned int *) addr &= ~(1 << offset);
	 return;
}
*/

void static gpio_set(unsigned int offset)
{
	unsigned int value = *((volatile unsigned int *) viraddr );
//	printf("[1]%s:0x%x=%x\n",__func__,value,offset);
	value = value & 0xfffffff0;
	value |= offset;
//	printf("[2]%s:0x%x=%x\n",__func__,value,offset);
	 *((volatile unsigned int *) viraddr) = value;
	 return;
}

static int LED_Init()
{
	if(mapfd < 0)
	{
		mapfd = open("/dev/mem", O_RDWR | O_SYNC);
		if (mapfd == -1)
		{
			LOGEX("devmap fail!\n");
			return -1;
		}
	}
	 void *mem_map =mmap(0, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, mapfd, GPIO1_BASE);
	 if(mem_map == NULL)
		 return -1;
	 *((volatile unsigned int *) (mem_map + 0x134)) &= ~(0xf);
	viraddr = mem_map + 0x13c;
	return 0;
}

int set_led_status(int status)
{
	 pthread_mutex_lock(&led_mutex);
	 led_status = status;
	 pthread_mutex_unlock(&led_mutex);
	 return 0;
}

static int change_led_status(int status)
{
	gpio_set(status & 0xf);
	return 0;
}

void* check_led_status(void* arg)
{
	int leds = 0;
	int led_inverse = 0;
	//int led
	LED_Init();
	while(1)
	{
		 pthread_mutex_lock(&led_mutex);
		 leds = led_status;
		 pthread_mutex_unlock(&led_mutex);

//		 printf("led status[0x%02x]\n",led_status);
		switch (leds)
		{
		case LED_NONE:
			change_led_status(~0);
			break;
		case LED_SIM_ERROR:
			if(led_inverse)
			{
				led_inverse = 0;
				change_led_status(~0);
			}else{
				led_inverse = 1;
				change_led_status(~2);
			}
			break;
		case LED_LTE_ERROR:
			if(led_inverse)
			{
				led_inverse = 0;
				change_led_status(~0);
			}else{
				led_inverse = 1;
				change_led_status(~1);
			}
			break;
		case LED_CSQ1:
			change_led_status(~1);
			break;
		case LED_CSQ2:
			change_led_status(~3);
			break;
		case LED_CSQ3:
			change_led_status(~7);
			break;
		case LED_CSQ4:
			change_led_status(~15);
			break;
		}
		usleep(500*1000);
	}
	return NULL;
}
