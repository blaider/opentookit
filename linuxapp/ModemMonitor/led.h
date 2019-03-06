/*************************************************************************
	> File Name: led.h
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Mon 05 Nov 2018 03:07:31 PM CST
 ************************************************************************/
#ifndef _LED_H_
#define _LED_H_

enum{
	LED_NONE = 0,
	LED_SIM_ERROR = 1,
	LED_LTE_ERROR = 2,
	LED_CSQ1	= 3,
	LED_CSQ2	= 4,
	LED_CSQ3	= 5,
	LED_CSQ4	= 6,
};

int set_led_status(int status);
void* check_led_status(void* arg);

#endif

