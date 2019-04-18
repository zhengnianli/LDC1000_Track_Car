#include "LED.H"

void LED_GPIO_Config(void)
{
  GPIO_InitTypeDef IO_Init;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD , ENABLE);	
  IO_Init.GPIO_Pin = GPIO_Pin_5;				    
  IO_Init.GPIO_Mode = GPIO_Mode_Out_PP;
  IO_Init.GPIO_Speed = GPIO_Speed_50MHz;	
  GPIO_Init(GPIOB, &IO_Init);					 
  IO_Init.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_3;	
  GPIO_Init(GPIOD, &IO_Init);
}
