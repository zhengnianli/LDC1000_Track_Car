#ifndef __ADC_H
#define __ADC_H

#define ADC1_DR_Address    ((unsigned int)0x40012400+0x4c)

void ADC1_GPIO_Init(void);
void ADC1_Mode_Config(void);

#endif
