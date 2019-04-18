#ifndef __PWM_H
#define __PWM_H
#include "stdint.h"
void PWM_TIM4_CH1_Config(unsigned short int Auto_Reload,unsigned short int Clock_Div);

void GPIO_Motor(void);
void GPIO_TimPWM(void);
void pwm1(uint16_t pulse);
void pwm2(uint16_t pulse);

#endif
