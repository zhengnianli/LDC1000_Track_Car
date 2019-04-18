/*-------------------------------------------------------------------------
       外部IO口中断配置程序
 说明: EXTI_Config函数配置IO口模式,具体的一些注意事项已经在注释中说明。
	   NVIC_Config函数配置中断向量表和优先级等,注意事项已经在注释说明。
 提醒: 中断服务程序写在stm32f10x_it.c中
	   中断服务函数名不能乱写,只能按照是哪个中断线就写哪个规定的中断函数名 
       具体的中断函数名请到startup_stm32f10x_hd.s文件中62行寻找相应的名字。
	   中断名在stm32f10x.h文件中188行可以找到各种中断名列表,根据需要选择
 时间: 2013年7月22日
--------------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "EXTI.H"

void EXTI_Config(void)
{
	EXTI_InitTypeDef EXTI_IO_Init;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO,ENABLE);
	/* 配置中断IO口需要使能两个时钟 */
	NVIC_Config();  //配置中断向量控制寄存器
	/* 多个IO口中断不能一起配置,只能如下分开配置 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource5); //配置中断线
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource2); 
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource3);

	EXTI_IO_Init.EXTI_Line = EXTI_Line5 | EXTI_Line2 | EXTI_Line3; 
	/* 如果有多个外部中断IO口需要配置可以使用 | 运算进行统一配置 */
	EXTI_IO_Init.EXTI_Mode = EXTI_Mode_Interrupt;  //中断模式
	EXTI_IO_Init.EXTI_Trigger = EXTI_Trigger_Falling;  //下降沿触发中断
	EXTI_IO_Init.EXTI_LineCmd = ENABLE;		//使能中断
	EXTI_Init(&EXTI_IO_Init);	 //写入配置
}

static void NVIC_Config(void)
{
	/* 如果要配置多个中断向量寄存器只能分开配置如下,不能一起配置 */
	NVIC_InitTypeDef NVICInit;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	//抢占响应优先级组1
	//配置PC5端口
	/* 中断名在stm32f10x.h文件中188行可以找到各种中断名列表,根据需要选择 */
	NVICInit.NVIC_IRQChannel = EXTI9_5_IRQn;//0-4独立IV,5-9共用EXTI9_5_IRQn IV,10-15共用EXTI15_10_IRQn IV	
	NVICInit.NVIC_IRQChannelPreemptionPriority = 0;  //抢占优先级0
    NVICInit.NVIC_IRQChannelSubPriority = 0;  		 //响应优先级0
	NVICInit.NVIC_IRQChannelCmd = ENABLE;	         //使能中断
	NVIC_Init(&NVICInit);                            //写入配置
	//配置PC2端口
	NVICInit.NVIC_IRQChannel = EXTI2_IRQn;
	NVICInit.NVIC_IRQChannelPreemptionPriority = 0;  //抢占优先级0
    NVICInit.NVIC_IRQChannelSubPriority = 0;         //响应优先级0
	NVICInit.NVIC_IRQChannelCmd = ENABLE;	  		 //使能中断
	NVIC_Init(&NVICInit); 							 //写入配置
	//配置PC3端口
	NVICInit.NVIC_IRQChannel = EXTI3_IRQn;
	NVICInit.NVIC_IRQChannelPreemptionPriority = 0;  //抢占优先级0
    NVICInit.NVIC_IRQChannelSubPriority = 0;  		 //响应优先级0
	NVICInit.NVIC_IRQChannelCmd = ENABLE;	  	 	 //使能中断
	NVIC_Init(&NVICInit); 							 //写入配置
}
