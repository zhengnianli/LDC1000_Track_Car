#ifndef __timer_H
#define __timer_H

#include "stm32f10x.h"

#define ENCODER_TIM_PERIOD (u16)(65000)   // number of pulses per revolution
//  uint16_t Sec=0,Min=0;
extern uint16_t Sec,Min;
extern int Q;
void Encoder_Init(void);
void readEncoder(void);
void TIM1_Int_Init(u16 arr,u16 psc);
#endif
