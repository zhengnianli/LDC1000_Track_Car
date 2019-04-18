/*-----------------------------------------------
		独立按键端口配置及检测程序
	说明:	已经将端口配置和按键扫描程序封装成型
	        根据程序实际需要更改相应的参数即可。
	时间:	2013年7月22日
------------------------------------------------*/
#include "indkey.h"
#include "stm32f10x.h"
#include "delay.h"

void Key_IO_Init(void)	 //按键IO配置
{
  GPIO_InitTypeDef IO_Init;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);	
  IO_Init.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_2 | GPIO_Pin_3;				    
  IO_Init.GPIO_Mode = GPIO_Mode_IPU;  //PC5、2、3上拉输入	
  GPIO_Init(GPIOC, &IO_Init);
}
/*-----------------------------------------------
	函数功能:	独立按键检测
	函数参数:	端口组名GPIOx,引脚名GPIO_Pin
	函数回值:	INT8U类型 按键值0,1
------------------------------------------------*/
unsigned char Key_Scan(GPIO_TypeDef* GPIOx,unsigned short int GPIO_Pin)
{
	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_PRESSED)  //按下接通低电平
	{
		delayms(3);
		if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_PRESSED)
		{
			while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_PRESSED);//等待按键释放
			return KEY_PRESSED;
		}
		else
			return KEY_UNPRESS;
	}
	else
		return KEY_UNPRESS;
}
