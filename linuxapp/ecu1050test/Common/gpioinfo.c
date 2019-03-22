#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdint.h>

#include "ioctls.h"
#include "gpioinfo.h"

static int gpio_fd = -1;
static unsigned int gpio_map = 0;

int gpio_init(int index)
{
	if(index >= sizeof(gpio_map)*8)
		index = 0;
	char devName[PATH_MAX];


//	printf("%s:map:%x,index:%d\n",__func__,gpio_map,index);
	if(gpio_fd > 0)
	{
		gpio_map |= 1<<index;
		return 0;
	}


	sprintf(devName,"/dev/%s",DEVICE_NODE_NAME);

	if(access(devName,F_OK))
	{
		printf("please install gpioinfo driver first\n");
		return -1;
	}
	gpio_fd = open(devName,O_RDWR);
	if(gpio_fd < 0)
	{
		printf("open device error");
		return -2;
	}
	gpio_map |= 1<<index;
	return 0;

}
int gpio_find(uint32_t port,uint32_t offset)
{
	int ret = 0;
	if(gpio_fd < 0)
	{
		printf("%s:please run gpio_init() first\n",__func__);
		return -1;
	}

	GPIO_GET_INDEX index;
	index.pyaddr = 0;
	index.port = port;
	index.offset = offset;
	ret = ioctl(gpio_fd,IOCTL_GPIO_GET_INDEX,&index);
	if(ret != 0)
		return -1;

	return index.index;

}
int gpmc_find(uint32_t phycial_addr)
{
	int ret = 0;
	if(gpio_fd < 0)
	{
		printf("%s:please run gpio_init() first\n",__func__);
		return -1;
	}
	GPIO_GET_INDEX index;
	index.pyaddr = phycial_addr;
	index.port = 0;
	index.offset = 0;
	ret = ioctl(gpio_fd,IOCTL_GPIO_GET_INDEX,&index);
	if(ret != 0)
		return -1;

	return index.index;

}
int gpio_setvalue(uint32_t index,uint32_t value)
{
	int ret = 0;
	GPIO_VALUE gvalue;
	if(gpio_fd < 0)
	{
		printf("%s:please run gpio_init() first\n",__func__);
		return -1;
	}

	gvalue.index = index;
	gvalue.value = value;
	ret = ioctl(gpio_fd,IOCTL_GPIO_SET_VALUE,&gvalue);
	if(ret < 0)
		printf("set value failed\n");
	return ret;
}
int gpio_getvalue(uint32_t index)
{
	int ret = 0;
	GPIO_VALUE gvalue;
	if(gpio_fd < 0)
	{
		printf("%s:please run gpio_init() first\n",__func__);
		return -1;
	}

	gvalue.index = index;
	gvalue.value = 0;
	ret = ioctl(gpio_fd,IOCTL_GPIO_GET_VALUE,&gvalue);
	if(ret < 0)
		printf("get value failed\n");
	return gvalue.value;
}
int gpio_uninit(int index)
{
	if(index >= sizeof(gpio_map)*8)
		index = 0;
//	printf("%s:map:%x,index:%d\n",__func__,gpio_map,index);
	gpio_map &= ~(1<<index);
	if(gpio_fd < 0)
	{
		printf("%s:please run gpio_init() first\n",__func__);
		return -1;
	}
	if(gpio_fd > 0 && gpio_map==0)
	{
		close(gpio_fd);
		gpio_fd = -1;
	}

	return 0;
}


