#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"
#define ON 1
#define OFF 0
#define LED1(a) if(a) \
			    GPIOB->BSRR = GPIO_Pin_5;\
			    else \
			    GPIOB->BRR = GPIO_Pin_5;

#define LED2(a) if(a) \
			    GPIOD->BSRR = GPIO_Pin_3;\
			    else \
			    GPIOD->BRR = GPIO_Pin_3;

#define LED3(a) if(a) \
			    GPIOD->BSRR = GPIO_Pin_6;\
			    else \
			    GPIOD->BRR = GPIO_Pin_6;

void LED_GPIO_Config(void);

#endif

