#include "timer.h"
#include "stm32f10x.h"
#include "usart1.h"
#include "stdio.h"
uint16_t Sec=0,Min=0;

int mSecond=0;
int Q=0;
int Encoder_Left;

void TIM1_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //时钟使能
	
	//定时器TIM1初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE ); //使能指定的TIM1中断,允许更新中断
 
	//中断优先级NVIC设置
  NVIC_InitStructure.NVIC_IRQChannel =  TIM1_UP_IRQn;;//指定中断源
 	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);            //选择组1
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
	
	TIM_Cmd(TIM1, ENABLE);  //使能TIMx					 
}



void Encoder_Init(void)
{
	
 TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
 TIM_ICInitTypeDef TIM_ICInitStructure; 
 GPIO_InitTypeDef GPIO_InitStructure;
 NVIC_InitTypeDef NVIC_InitStructure;
 
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  //时钟使能
 
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//时钟使能
 
 GPIO_StructInit(&GPIO_InitStructure);
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1;   //配置IO口
 GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IN_FLOATING;   // 配置io口为浮空输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOA, &GPIO_InitStructure);
 
 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); //先占优先级2位从优先级2位
 NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; //TIM3全局中断
 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;  //设置先占优先级
 NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;    //设置从占优先级
 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  //中断使能
 NVIC_Init(&NVIC_InitStructure);

 TIM_DeInit(TIM2);                                //将外设TIMx寄存器重设为缺省值
 TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);  //每一个参数按缺省值填入
 TIM_TimeBaseStructure.TIM_Prescaler = 0x0;  // Noprescaling  //预分频器
 TIM_TimeBaseStructure.TIM_Period =ENCODER_TIM_PERIOD-1;  // 设定计数器自动重装值
 TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1;  //设置了时钟分割
 TIM_TimeBaseStructure.TIM_CounterMode =TIM_CounterMode_Up;  //TIM向上计数模式
 TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);     //初始化TIMx的时间基数单位
 
 TIM_EncoderInterfaceConfig(TIM2,TIM_EncoderMode_TI12,
                            TIM_ICPolarity_Rising, TIM_ICPolarity_Rising); //设置TIMx编码界面,使用TIM编码模式3
 TIM_ICStructInit(&TIM_ICInitStructure);                    //每一个参数按缺省值填入
 TIM_ICInitStructure.TIM_ICFilter = 10;  //ICx_FILTER;  //TIM_ICFilter选择输入比较滤波器
 TIM_ICInit(TIM2,&TIM_ICInitStructure);  //初始化外设TIMx
 
 // Clearall pending interrupts
 TIM_ClearFlag(TIM2, TIM_FLAG_Update);        //清除TIMx的待处理标志位
 TIM_ITConfig(TIM2, TIM_IT_Update,ENABLE);    //使能或者失能指定的TIM中断
  //Resetcounter
 TIM2->CNT = 0;
// ENC_Clear_Speed_Buffer();
 
 TIM_Cmd(TIM2, ENABLE);     //使能或者失能TIMx外设
}


/**************************************************************************
函数功能：读取编码器的数据并进行数据类型转换
入口参数：无
返回  值：无
**************************************************************************/
void readEncoder(void)
{
	 u16 Encoder_L;       //===左右编码器的脉冲计数

// 		Encoder_R = TIM4 -> CNT;       //===获取正交解码1数据	
// 		TIM4-> CNT=0;                 //===计数器清零  
	  Encoder_L= TIM2 -> CNT;        //===获取正交解码2数据	
	  TIM2 -> CNT=0;	               //===计数器清零
	
// 	  Encoder_Left=Encoder_L; 
	
 		if(Encoder_L>32768)  Encoder_Left=Encoder_L-65000; 
	   else  Encoder_Left=Encoder_L;  
	  //=这个处理的原因是：编码器到0后会跳到65000向下计数，这样处理方便我们在控制程序中使用
// 	  if(Encoder_R>32768)  Encoder_Right=Encoder_R-65000; else  Encoder_Right=Encoder_R;
//    	Encoder_Left=-Encoder_Left;//这里取反是因为，平衡小车的两个电机是旋转了180度安装的，为了保证前进后退时候的编码器数据符号一致
}


//定时器1中断服务程序
void   TIM1_UP_IRQHandler(void)   //TIM1中断
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
		
	{
		  	TIM_ClearITPendingBit(TIM1, TIM_IT_Update  );  //清除TIMx更新中断标志 
// 		  	readEncoder(); 
// 				printf("左编码器计数 %d  \r\n",Encoder_Left);
		
		  mSecond++ ;	
		   if( mSecond<20 ) 			// 判断是否计满1秒
		     return ;
	      mSecond= 0;
		    Sec++ ;

	     if( Sec< 60 ) 				// 判断是否计满60秒
		   return ;
	     Sec= 0 ;
	     Min++ ;
	    if( Min< 60 ) 				// 判断是否计满60分
		return ;
	    Min= 0 ;
		Q=0.076*(60*Min+Sec)*1000;
	
	}
}


