/*
 * =====================================================================================
 *
 *       Filename:  simulatetest.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年12月10日 20时48分08秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

//eventX代表的是所有输入设备(input核心)的事件，比如按键按下，
//或者鼠标移动，或者游戏遥控器等等，
//在系统查看的方法是  cat /proc/bus/input/devices
//就可以看到每个eventX是什么设备的事件了。


//#define EV_SYN          0x00    表示设备支持所有的事件
//#define EV_KEY          0x01    键盘或者按键，表示一个键码
//#define EV_REL          0x02    鼠标设备，表示一个相对的光标位置结果
//#define EV_ABS          0x03    手写板产生的值，其是一个绝对整数值
//#define EV_MSC          0x04    其他类型
//#define EV_LED          0x11    LED灯设备
//#define EV_SND          0x12    蜂鸣器，输入声音
//#define EV_REP          0x14    允许重复按键类型
//#define EV_PWR          0x16    电源管理事件
//#define EV_FF_STATUS 0x17
//#define EV_MAX 0x1f
//#define EV_CNT (EV_MAX+1)

//value:根据Type的不同而含义不同。
//例如：
//Type为EV_KEY时，value: 0表示按键抬起。1表示按键按下。（4表示持续按下等？）。
//Type为EV_REL时，value:　表明移动的值和方向（正负值）。
//Type为EV_ABS时，code表示绝对位置。


//其中0表示释放，1按键按下，2表示一直按下
//0 for EV_KEY for release, 1 for keypress and 2 for autorepeat.
void simulate_key(int fd, int kval)
{
	struct input_event event;
	event.type = EV_KEY;
	event.value = 1;
	event.code = kval;
	gettimeofday(&event.time, 0);
	write(fd, &event, sizeof(event));
	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(fd, &event, sizeof(event));

	memset(&event, 0, sizeof(event));
	gettimeofday(&event.time, NULL);
	event.type = EV_KEY;
	event.code = kval;
	event.value = 0;
	write(fd, &event, sizeof(event));
	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(fd, &event, sizeof(event));
}
void simulate_mouse(int fd)
{
	struct input_event event;
	memset(&event, 0, sizeof(event));
	gettimeofday(&event.time, NULL);
	event.type = EV_REL;
	event.code = REL_X;
	event.value = 10;
	write(fd, &event, sizeof(event));
	event.type = EV_REL;
	event.code = REL_Y;
	event.value = 10;
	write(fd, &event, sizeof(event));
	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(fd, &event, sizeof(event));
}
void simulate_mouse1(int fd)
{
	struct input_event event;
	memset(&event, 0, sizeof(event));
	gettimeofday(&event.time, NULL);
	event.type = EV_REL;
	event.code = REL_X;
	event.value = -10;
	write(fd, &event, sizeof(event));
	event.type = EV_REL;
	event.code = REL_Y;
	event.value = -10;
	write(fd, &event, sizeof(event));
	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(fd, &event, sizeof(event));
}
int main()
{
	int fd_kbd;
	int fd_mouse;
	fd_kbd = open("/dev/input/event3", O_RDWR);
	if (fd_kbd <= 0)
	{
		perror("error open keyboard");
		return -1;
	}
	fd_mouse = open("/dev/input/event4", O_RDWR);
	if (fd_mouse <= 0)
	{
		perror("error open mouse");
		return -2;
	}
	int i = 0;
	for (i = 0; i < 10; i++)
	{
		simulate_key(fd_kbd, KEY_A + i);
		simulate_mouse(fd_mouse);
		sleep(1);
	}
	for (i = 0; i < 10; i++)
	{
		simulate_key(fd_kbd, KEY_A + i);
		simulate_mouse1(fd_mouse);
		sleep(1);
	}
	close(fd_kbd);
}
