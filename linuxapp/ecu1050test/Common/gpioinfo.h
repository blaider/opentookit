#ifndef GPIOINFO_H_
#define GPIOINFO_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


int gpio_init(int index);
int gpio_find(uint32_t port,uint32_t offset);
int gpmc_find(uint32_t phycial_addr);
int gpio_setvalue(uint32_t index,uint32_t value);
int gpio_getvalue(uint32_t index);
int gpio_uninit(int index);


enum {
	GPIO_LED_RUN,
	GPIO_LED_ERR,
	GPIO_LED_PROG,
	GPIO_LED1,
	GPIO_LED2,
	GPIO_LED3,
	GPIO_LED4,
	GPIO_BOARDINFO,
	GPIO_DIO,
	GPIO_WDT,
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GPIOINFO_H_ */
