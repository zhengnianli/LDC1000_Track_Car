#include "duoji.h"
#include "stm32f10x.h"
#include "stdint.h"
/*
*********************************************************************************************************
*	函 数 名: GPIO_Tim3CH2PWM
*	功能说明: 定时器3通道2的GPIO初始化，用于产生pwm波控制舵机
*	形    参: 无
*	返 回 值: 无                            PA7 <-> TIM3_CH2
*********************************************************************************************************
*/
void GPIO_Tim3CH2PWM(void)
{
 GPIO_InitTypeDef GPIO_InitStructure;
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO , ENABLE);//
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;         //TIM_CH2  GPIOA_Pin_7输出
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOA, &GPIO_InitStructure);        //不解释
}
/*
*********************************************************************************************************
*	函 数 名: TIM3_CH2_Configuration
*	功能说明: TIM3配置，产生pwm波控制电机
*	形    参: pulse
*	返 回 值: 无						Psc=100,Arr=14400-1
*********************************************************************************************************
*/
void TIM3_CH2_Configuration(uint16_t pulse)
{
 TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 TIM_OCInitTypeDef  TIM_OCInitStructure;

 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  //使能TIM3
 
//假如我要产生50Hz占空比为50%的PWM波，方便控制舵机
//设置方法：
//系统默认时钟为72MHz，预分频100次，得到TIM3计数时钟为720000Hz
//计数长度为14400，可得PWM频率为720000/14400=50Hz
//具体设置可参考上面的设置方法，简单明了!
 
 TIM_TimeBaseStructure.TIM_Prescaler = 100;
 TIM_TimeBaseStructure.TIM_Period = 14400-1;    
 TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
                     
 TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式

 TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);  
 TIM_ARRPreloadConfig(TIM3, DISABLE);  //禁止ARR预装载缓冲器,也可以不用设置

 TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
 
 //占空比设置方法：
 //占空比=(TIM_Pulse+1)/(TIM_Period+1)
 
 TIM_OCInitStructure.TIM_Pulse =pulse; //1600对应90度，1200对应45度 ，900对应0度 ，600对应-45度 ，300对应-90度   
 TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;    //输出极性:TIM输出比较极性低
 TIM_OC2Init(TIM3, &TIM_OCInitStructure);          
 TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);           //使能TIM3在CCR2上的预装载寄存器

 TIM_CtrlPWMOutputs(TIM3,ENABLE);      //设置TIM3的PWM输出为使能
 TIM_ARRPreloadConfig(TIM3, ENABLE);   //使能TIM3在ARR上的预装载寄存器 
 TIM_Cmd(TIM3, ENABLE);                //使能TIM3外设
}