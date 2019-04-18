#include "motor.h"
#include "stm32f10x.h"
#include "stdint.h"
#include "pwm.h"
uint16_t LdcVal=4000,LDCValMax;
uint16_t LdcValTmp=0;

/*
		  左轮 pwm1 <-> PB0 (Tim3_CH3)			右轮 pwm3 <-> PB8(Tim4_CH3)
		       pwm2 <-> PB1 (Tim3_CH4)               pwm4 <-> PB9(Tim4_CH4)		  
*/
/*
*********************************************************************************************************
*	函 数 名: Run
*	功能说明: 前进函数
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void Run(void)
{
	/*左轮*/
	GPIO_SetBits(GPIOC,GPIO_Pin_6);
	GPIO_ResetBits(GPIOC,GPIO_Pin_7);
	/*右轮*/
	GPIO_SetBits(GPIOC,GPIO_Pin_8);
	GPIO_ResetBits(GPIOC,GPIO_Pin_9);
}
/*
*********************************************************************************************************
*	函 数 名: RunLeft
*	功能说明: 左转函数
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void RunLeft(void)
{
	/*左轮*/
	GPIO_ResetBits(GPIOC,GPIO_Pin_6);
	GPIO_ResetBits(GPIOC,GPIO_Pin_7);
	/*右轮*/
	GPIO_SetBits(GPIOC,GPIO_Pin_8);
	GPIO_ResetBits(GPIOC,GPIO_Pin_9);
}
/*
*********************************************************************************************************
*	函 数 名: RunLeft
*	功能说明: 左转函数
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void RunRight(void)
{
	/*左轮*/
	GPIO_SetBits(GPIOC,GPIO_Pin_6);
	GPIO_ResetBits(GPIOC,GPIO_Pin_7);
	/*右轮*/
	GPIO_ResetBits(GPIOC,GPIO_Pin_8);
	GPIO_ResetBits(GPIOC,GPIO_Pin_9);
}

