/*---------------------------------------------------------
		  ADC配置驱动程序
	说明: 先使能IO口为模拟输入,接着配置DMA通道,注意声明一个
	      DMA使用的全局变量short类型,目前配置的是用PC1做AD
		  输入,对应的AD转换通道为ADC123_IN11,具体哪个IO口对
		  应哪个AD转换通道请参考F10x数据手册20页引脚定义。
	时间: 2013年7月24日 BoX编写 
---------------------------------------------------------*/
#include "stm32f10x.h"
#include "adc.h"

unsigned short int ADC1_DMA_Value;

void ADC1_GPIO_Init(void)
{
	GPIO_InitTypeDef IO_Init;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC , ENABLE);
	//C1模拟输入
	IO_Init.GPIO_Pin = GPIO_Pin_1;
	IO_Init.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC,&IO_Init);
}

/* #define ADC1_DR_Address ((unsigned int)0x40012400+0x4c) */
/* unsigned short int ADC1_DMA_Value; */

void ADC1_Mode_Config(void)
{
	//配置DMA
	DMA_InitTypeDef DMA_csh;
	ADC_InitTypeDef ADC_csh;

	DMA_DeInit(DMA1_Channel1);  //DMA复位,通道1
	DMA_csh.DMA_PeripheralBaseAddr = ADC1_DR_Address;  //ADC1地址
	DMA_csh.DMA_MemoryBaseAddr = (unsigned int) &ADC1_DMA_Value;  //内存地址
	/* 数据从ADC1->DR寄存器通过DMA转移到内存并且保存在变量ADC_DMA_Value中 */
	DMA_csh.DMA_DIR = DMA_DIR_PeripheralSRC;  //外设为数据源
	DMA_csh.DMA_BufferSize = 1;  //缓冲大小
	DMA_csh.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址固定
	DMA_csh.DMA_MemoryInc = DMA_MemoryInc_Disable; //内存地址固定
	DMA_csh.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //DMA外设数据大小 半字
	DMA_csh.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //DMA内存数据大小 半字
	DMA_csh.DMA_Mode = DMA_Mode_Circular;  //循环传输
	DMA_csh.DMA_Priority = DMA_Priority_High;  //DMA优先级高
	DMA_csh.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1,&DMA_csh);  //写入DMA1配置参数
	DMA_Cmd(DMA1_Channel1,ENABLE);	   //使能DMA1通道1
	//配置ADC
	ADC_csh.ADC_Mode = ADC_Mode_Independent;  //独立ADC模式
    ADC_csh.ADC_ScanConvMode = DISABLE;   //关闭扫描模式
	ADC_csh.ADC_ContinuousConvMode = ENABLE;  //连续AD转换开启
	ADC_csh.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  //关闭外部触发转换
	ADC_csh.ADC_DataAlign = ADC_DataAlign_Right; //数据右对齐
	ADC_csh.ADC_NbrOfChannel = 1;  //要转换的通道数目1
	ADC_Init(ADC1,&ADC_csh);   //写入ADC1配置参数
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);  //PCLK2 = 72MHz 72DIV8 = 9MHz ,AD时钟最大14MHz
	ADC_RegularChannelConfig(ADC1,ADC_Channel_11,1,ADC_SampleTime_55Cycles5); //规则通道配置
               	     /* ADCx , ADC转换通道 , ADC扫描转换次序 , ADC采样周期 */
	ADC_DMACmd(ADC1 , ENABLE); //使能ADC1 DMA
	ADC_Cmd(ADC1 , ENABLE); //使能ADC1
	ADC_ResetCalibration(ADC1);	  //复位校准寄存器
	while(ADC_GetResetCalibrationStatus(ADC1)); //等待校准寄存器复位完成
	ADC_StartCalibration(ADC1);	  //开始校准
	while(ADC_GetCalibrationStatus(ADC1)); //等待校准完成
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	 //ADC1软件触发转换开启
}

