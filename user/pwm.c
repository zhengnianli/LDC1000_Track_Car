/*---------------------------------------
			PWM配置程序
----------------------------------------*/
#include "pwm.h"
#include "stm32f10x.h"
/*
*********************************************************************************************************
*	函 数 名: GPIO_Motor                    
*	功能说明: 电机正反向                        
*	形    参: 无
*	返 回 值: 无                             
*********************************************************************************************************
*/
void GPIO_Motor(void)
{
	 GPIO_InitTypeDef GPIO_InitStructure;
	
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);
	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;           
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_Init(GPIOC, &GPIO_InitStructure);       
}
/*
*********************************************************************************************************
*	函 数 名: GPIO_TimPWM                       左PB0 <-> TIM3_CH3      右PB1 <-> TIM3_CH4  
*	功能说明: 产生四路PWM控制电机                        
*	形    参: 无
*	返 回 值: 无                             
*********************************************************************************************************
*/
void GPIO_TimPWM(void)
{
	 GPIO_InitTypeDef GPIO_InitStructure;
	
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO , ENABLE);
	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;           
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;     
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_Init(GPIOB, &GPIO_InitStructure);       
}
/*
*********************************************************************************************************
*	函 数 名: pwm1
*	功能说明: TIM3配置，产生pwm波控制电机
*	形    参: pulse
*	返 回 值: 无						
*********************************************************************************************************
*/
void pwm1(uint16_t pulse)
{
	 TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	 TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  
	 
	 TIM_TimeBaseStructure.TIM_Prescaler = 0;
	 TIM_TimeBaseStructure.TIM_Period = 7200-1;    
	 TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;					 
	 TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;    //TIM向上计数模式
	 TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);  
	 TIM_ARRPreloadConfig(TIM3, DISABLE);                           //禁止ARR预装载缓冲器,也可以不用设置

	 TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;              //选择定时器模式:TIM脉冲宽度调制模式2
	 TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  //比较输出使能
	 
	 TIM_OCInitStructure.TIM_Pulse =pulse;    
	 TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性高
	 TIM_OC3Init(TIM3, &TIM_OCInitStructure);                 //根据TIM_OCInitStruct中指定的参数初始化外设TIM3
	 TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);        //使能TIM3在CCR2上的预装载寄存器

	 TIM_CtrlPWMOutputs(TIM3,ENABLE);    //设置TIM3的PWM输出为使能
	 TIM_ARRPreloadConfig(TIM3, ENABLE); //使能TIM3在ARR上的预装载寄存器 
	 TIM_Cmd(TIM3, ENABLE);              //使能TIM3外设
}
/*
*********************************************************************************************************
*	函 数 名: pwm2
*	功能说明: TIM3配置，产生pwm波控制电机
*	形    参: pulse
*	返 回 值: 无						
*********************************************************************************************************
*/
void pwm2(uint16_t pulse)
{
	 TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	 TIM_OCInitTypeDef  TIM_OCInitStructure;

	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  
	 
	 TIM_TimeBaseStructure.TIM_Prescaler = 0;
	 TIM_TimeBaseStructure.TIM_Period = 7200-1;    
	 TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;					 
	 TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;    //TIM向上计数模式
	 TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);  
	 TIM_ARRPreloadConfig(TIM3, DISABLE);                           //禁止ARR预装载缓冲器,也可以不用设置

	 TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;              //选择定时器模式:TIM脉冲宽度调制模式2
	 TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  //比较输出使能
	 
	 TIM_OCInitStructure.TIM_Pulse =pulse;    
	 TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性高
	 TIM_OC4Init(TIM3, &TIM_OCInitStructure);                 //根据TIM_OCInitStruct中指定的参数初始化外设TIM3
	 TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);        //使能TIM3在CCR2上的预装载寄存器

	 TIM_CtrlPWMOutputs(TIM3,ENABLE);    //设置TIM3的PWM输出为使能
	 TIM_ARRPreloadConfig(TIM3, ENABLE); //使能TIM3在ARR上的预装载寄存器 
	 TIM_Cmd(TIM3, ENABLE);              //使能TIM3外设
}
/*---------------------------------------
 函数功能: 定时器4通道1配置PWM定时器
 函数参数: 输出8MHz时钟给TBCLK,LDC1000需要8MHz时钟方可运行
----------------------------------------*/
void PWM_TIM4_CH1_Config(unsigned short int Auto_Reload,unsigned short int Clock_Div)
{	
	GPIO_InitTypeDef IO_Init;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);	 //使能定时器4时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	 //使能AFIO时钟
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);	
 	
	IO_Init.GPIO_Pin = GPIO_Pin_6;				    
  	IO_Init.GPIO_Mode = GPIO_Mode_AF_PP;				 //PB6复用推挽输出
  	IO_Init.GPIO_Speed = GPIO_Speed_50MHz;	
  	GPIO_Init(GPIOB, &IO_Init);

	TIM_TimeBaseStructure.TIM_Prescaler =  Clock_Div; 			//时钟预分频
	TIM_TimeBaseStructure.TIM_CounterMode =  TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseStructure.TIM_Period = Auto_Reload;				//自动重装值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	    //时钟分频1
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode =  TIM_OCMode_PWM2;   //选择定时器模式:PWM2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM4,&TIM_OCInitStructure);				 //根据指定的参数初始化外设TIM3 OC2
	
	TIM_OC1PreloadConfig(TIM4,TIM_OCPreload_Enable); //使能TIM4在CCR2上的预装载寄存器
	TIM_SetCompare1(TIM4,4);
	TIM_Cmd(TIM4, ENABLE);
}



