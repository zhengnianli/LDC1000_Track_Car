#ifndef __motor_H
#define __motor_H

#include "stm32f10x.h"


extern uint16_t LdcValTmp;  //读回来的值

void Run(void);
void RunRight(void);
void RunLeft(void);
#endif
