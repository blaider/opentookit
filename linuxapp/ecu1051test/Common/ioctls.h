#ifndef IOCTLS_H_
#define IOCTLS_H_

#include <linux/ioctl.h>

#define DEVICE_NODE_NAME "gpioinfo"

typedef struct _GPIO_GET_INDEX{
	int32_t index;
   uint32_t pyaddr;
   uint32_t	port;
   uint32_t offset;
}GPIO_GET_INDEX;

#define IOCTL_GPIO_GET_INDEX	_IOWR('a',0,int) //

typedef struct _GPIO_VALUE{
   uint32_t index;
   uint32_t value;
}GPIO_VALUE;
#define IOCTL_GPIO_GET_VALUE	_IOWR('a',1,int)
#define IOCTL_GPIO_SET_VALUE	_IOWR('a',2,int)
#endif /* IOCTLS_H_ */
