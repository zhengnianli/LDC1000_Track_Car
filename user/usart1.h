#ifndef __USART1_H
#define __USART1_H

#include <stdio.h>
#include <stdarg.h>

void USART1_Config(void);
void NVIC_Config(void);
int fputc(int ch,FILE *f);
void USART1_printf(USART_TypeDef * USARTx, unsigned char *Data,...);
static char *itoa(int value, char *string, int radix);

#endif
