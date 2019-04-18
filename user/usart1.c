/*---------------------------------------------------------
	      串口1配置程序
	说明: 串口配置程序需要包含stdarg.h方可使用
	      奋斗板接口PA10->RXD  PA9->TXD,USART1
		  使用printf的时候加\r\n方可输出回车键
		  如果需要使用串口中断请添加misc.c文件
		  并且包含misc.h头文件,配置NVIC中断源
		  到stm32f10x_it.c编写中断服务程序
---------------------------------------------------------*/
#include "stm32f10x.h"
#include "usart1.h"
#include "misc.h"   //如果需要用中断则包含此头文件
#include <stdarg.h>

void USART1_Config(void)
{
 	GPIO_InitTypeDef IO_Init;
	USART_InitTypeDef USART_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA , ENABLE);
	NVIC_Config(); //用到串口中断,配置中断参数
	//A9复用推挽输出
	IO_Init.GPIO_Pin = GPIO_Pin_9;
	IO_Init.GPIO_Mode = GPIO_Mode_AF_PP;
	IO_Init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&IO_Init);
	//A10浮空输入
	IO_Init.GPIO_Pin = GPIO_Pin_10;
	IO_Init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&IO_Init);
	//配置串口1
	USART_InitStructure.USART_BaudRate = 115200;  //设置波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//八位数据位
	USART_InitStructure.USART_StopBits =  USART_StopBits_1;		//1位停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;			//无校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //关硬件流
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	  //发送、接收模式
	USART_Init(USART1,&USART_InitStructure);  //写入配置
	USART_Cmd(USART1,ENABLE);				  //使能串口
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE); //使能接收中断
}

void NVIC_Config()
{
	/* 如果要配置多个中断向量寄存器只能分开配置如下,不能一起配置 */
	NVIC_InitTypeDef NVICInit;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	//抢占响应优先级组1
	//串口接收中断
	/* 中断名在stm32f10x.h文件中188行可以找到各种中断名列表,根据需要选择 */
	NVICInit.NVIC_IRQChannel = USART1_IRQn;	
	NVICInit.NVIC_IRQChannelPreemptionPriority = 2;  //抢占优先级2
    NVICInit.NVIC_IRQChannelSubPriority = 1;  		 //响应优先级1
	NVICInit.NVIC_IRQChannelCmd = ENABLE;	         //使能中断
	NVIC_Init(&NVICInit);                            //写入配置
}

/* 实现了printf函数的重定向作用 */
int fputc(int ch,FILE *f)
{
 	USART_SendData(USART1, (unsigned char) ch);	//调用stm32f10x_usart.c中的函数
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET);  //等待发送完毕
	return ch;
}
/*---------------------------------------------------------
	函数功能: 自定义的类似printf功能函数
	函数参数: 需要发送数据的串口号USARTx
			  需要打印的数据data
	返回参数: 无
---------------------------------------------------------*/
void USART1_printf(USART_TypeDef* USARTx, uint8_t *Data,...)
{
	const char *s;
	int d;   
	char buf[16];
	
	va_list ap;
	va_start(ap, Data);
	
	while ( *Data != 0)     // 判断是否到达字符串结束符
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //回车符
					USART_SendData(USARTx, 0x0d);
					Data ++;
				break;
				
				case 'n':							          //换行符
					USART_SendData(USARTx, 0x0a);	
					Data ++;
				break;
				
				default:
					Data ++;
				break;
			}			 
		}
		else if ( *Data == '%')
		{									  //
			switch ( *++Data )
			{				
				case 's':										  //字符串
					s = va_arg(ap, const char *);
					for ( ; *s; s++) 
					{
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
					}
					Data++;
				break;
			
				case 'd':										//十进制
					d = va_arg(ap, int);
					itoa(d, buf, 10);
					for (s = buf; *s; s++) 
					{
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
					}
					Data++;
				break;
			    default:  Data++;
				break;
			}		 
		} 
		else USART_SendData(USARTx, *Data++); //如果没遇到转意字符就正常输出
		while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
	}
}
/*--------------------------------------------------
	函数功能: 整形数据转换成字符串函数
	函数参数: 需要转换的整形数value
	          转换后的字符串 string
			  radix = 10
	函数返回: 无
---------------------------------------------------*/
static char *itoa(int value, char *string, int radix)
{
	int     i, d;
	int     flag = 0;
	char    *ptr = string;
	/* This implementation only works for decimal numbers. */
	if (radix != 10)
	{
	    *ptr = 0;
	    return string;
	}
	if (!value)
	{
	    *ptr++ = 0x30;
	    *ptr = 0;
	    return string;
	}
	/* if this is a negative value insert the minus sign. */
	if (value < 0)
	{
	    *ptr++ = '-';
	    /* Make the value positive. */
	    value *= -1;
	}
	for (i = 10000; i > 0; i /= 10)	//把每整数的每一位分别格式化成字符
	{
	    d = value / i;
	    if (d || flag)
	    {
	        *ptr++ = (char)(d + 0x30);
	        value -= (d * i);
	        flag = 1;
	    }
	}
	/* Null terminate the string. */
	*ptr = 0;
	return string;
}
