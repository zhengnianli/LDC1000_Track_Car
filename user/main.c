/*
*********************************************************************************************************
*	                                 LDC1000循迹小车
*	
*     					用到3路PWM波，TIM4_CH1_pwm用于产生8Mhz时钟，
*	  					TIM3_CH3_pwm用于控制左电机，
*	  					TIM3_CH4_pwm用于控制右电机，
*
*********************************************************************************************************
*/
#include "stm32f10x.h"
#include "usart1.h"
#include "delay.h"
#include "stdio.h"
#include "ldc1000.h"
#include "pwm.h"
#include "motor.h"
#include "oled.h"
#include "timer.h"
/*----------------------------------------
	   全局变量与函数的声明
----------------------------------------*/
extern uint8_t RegArray[REG_LEN];
uint16_t x,y,z;
int8_t  i = 0,n=0,m=0,h=0;
int reg21=0,reg22=0,reg2122=0;
char proximtyData[2];
int proximtyDataMIN;
int proximtyDataMAX;
int proximtyDataTEMP;

// float Q=0.0;//定义路程变量
int x1,x2,x3,x4,x5;

unsigned char SecBak;

uint16_t ReadValue(void);
void Ldc1000Test(void);
void Beep_GPIO_Config(void);
// void Lucheng(void);
/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: 程序入口
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int main(void)
{
	DelayInit();
	USART1_Config();
	PWM_TIM4_CH1_Config(8,0);	 //输出8MHz时钟给TBCLK,LDC1000需要8MHz时钟方可运行
	delayms(5);
	LDC1000_Init();
  	OLED_Init();			//初始化OLED  
 	OLED_Clear() ; 
 	OLEDShowCHinese(); 	
// 	Encoder_Init();
 	TIM1_Int_Init(499,7199);  
	delayms(5);
	GPIO_TimPWM();
	GPIO_Motor();
 	Run();
 	while(1)
    {	
		if( SecBak!=Sec )
		{
			SecBak=Sec;
			OLED_ShowNum(77,3,Sec,2,16);
			OLED_ShowNum(48,3,Min,2,16);
			OLED_ShowNum(40,6,Q,5,16);
		}
		LdcValTmp=ReadValue();
		printf("ProximityData90=%d\n",LdcValTmp);
		if((8200<LdcValTmp)&&(LdcValTmp<15000))
		{
			pwm1(2100);
			pwm2(5250);
		}
		else if((3000<LdcValTmp)&&(LdcValTmp<8200))
		{
			pwm1(4700);
			pwm2(2100);
		}
		else if(LdcValTmp==8200)
		{
			pwm1(5000);
			pwm2(5000);
		}
		else if(LdcValTmp>=15000)
		{
			delayms(3);
			y=ReadValue();
			if(y<LdcValTmp)
			{    
				GPIO_SetBits(GPIOB,GPIO_Pin_8);
			}
			else
			{
				pwm1(4500);
				pwm2(4500);			
				GPIO_ResetBits(GPIOB,GPIO_Pin_8);
				delayms(200);
				GPIO_SetBits(GPIOB,GPIO_Pin_8);	 
			}
		}
	}			
}
/*
*********************************************************************************************************
*	函 数 名: ReadValue
*	功能说明: Ldc读值函数
*	形    参: 无
*	返 回 值: reg2122
*********************************************************************************************************
*/
uint16_t ReadValue(void)
{
	SPI_LDC1000_ReadBytes(LDC1000_CMD_PROXLSB,&proximtyData[0],2);
    proximtyDataMAX = ((unsigned char) proximtyData[1]<<8) + proximtyData [0];//把两个寄存器中读到的值合并，下同
    proximtyDataMIN = proximtyDataMAX;
	for (i=0;i<100;i++)//读100次
    {
		SPI_LDC1000_ReadBytes(LDC1000_CMD_PROXLSB,&proximtyData[0],2);	
		proximtyDataTEMP = ((unsigned char)proximtyData[1]<<8) + proximtyData [0];
		if (proximtyDataTEMP < proximtyDataMIN)//小于最小的就把值赋给最小的
			proximtyDataMIN = proximtyDataTEMP;
		if (proximtyDataTEMP > proximtyDataMAX)//大于最大的就把值赋给最大的
			proximtyDataMAX = proximtyDataTEMP;
		reg2122=reg2122+(int)(proximtyDataTEMP);
    }
	reg2122=reg2122/100;

	return reg2122;	
}


// void Lucheng(void)
// {

// 	x1=(int)Q/10000;
// 	
// 	x2=(int)Q%10000/1000;
// 	
// 	x3=(int)Q%1000/100;
// 	
// 	x4=(int)Q%100/10;
// 	
// 	x5=(int)Q%10;

// }





