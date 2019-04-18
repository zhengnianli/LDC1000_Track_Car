#include "ldc1000.h"
#include "delay.h"
/*----------------------------------------
				全局变量
----------------------------------------*/
uint8_t RegArray[REG_LEN];
/*----------------------------------------
 				宏定义
----------------------------------------*/
#define CSB_0 	GPIOB->BRR  = GPIO_Pin_7
#define CSB_1 	GPIOB->BSRR = GPIO_Pin_7
/*----------------------------------------
 函数功能: LDC1000初始化
 函数参数: 无
----------------------------------------*/
void LDC1000_Init(void)
{
	LDC1000_GPIO_Init();
	LDC1000_SPI_Init();
	
	  delayus(5);
    LDC1000_WriteReg(LDC1000_CMD_RPMAX,     0x16);
    LDC1000_WriteReg(LDC1000_CMD_RPMIN,     0x38);
	LDC1000_WriteReg(LDC1000_CMD_SENSORFREQ,  0x94);
	LDC1000_WriteReg(LDC1000_CMD_LDCCONFIG,   0x17);
	LDC1000_WriteReg(LDC1000_CMD_CLKCONFIG,   0x02);
	LDC1000_WriteReg(LDC1000_CMD_INTCONFIG,   0x02);

	LDC1000_WriteReg(LDC1000_CMD_THRESHILSB,  0x50);
	LDC1000_WriteReg(LDC1000_CMD_THRESHIMSB,  0x14);
	LDC1000_WriteReg(LDC1000_CMD_THRESLOLSB,  0xC0);
	LDC1000_WriteReg(LDC1000_CMD_THRESLOMSB,  0x12);

	LDC1000_WriteReg(LDC1000_CMD_PWRCONFIG,   0x01);	 	
}
/*----------------------------------------
 函数功能: LDC1000 GPIO初始化
 函数参数: IO口连接图

 	+5V 	<-> 	+5V
	DCLK	<-> 	PB6 <-> TIM4_CH1
	SCLK    <-> 	PA5 <-> SPI1_SCK
	SDI     <-> 	PA7 <-> SPI1_MOSI
	SDO     <-> 	PA6 <-> SPI1_MISO
	VIO		<-> 	3V3
	GND		<-> 	GND
	INT     <-> 	PC2 <-> GPIO
	CSB		<-> 	PB7	<-> GPIO
	GND		<->		GND
----------------------------------------*/
void LDC1000_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//记住使能AFIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB| RCC_APB2Periph_GPIOC , ENABLE);
	
	//PB6->PWM_OUTPUT
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				    
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//PB6复用推挽输出
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
  	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//A5->SCK  A6->MISO A7->MOSI
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   //SPI用复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	//PC2->INT
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);

	//PB7->CSB
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}
/*----------------------------------------
 函数功能: SPI参数初始化
 函数参数: LDC1000的时钟频率必须在4.2MHz
 		   且不能超过5MHz,不能低于4MHz,否则
		   模块不能够正常通信,所以SPI波特率
		   分频系数选择16分频.
----------------------------------------*/
void LDC1000_SPI_Init(void)
{
	SPI_InitTypeDef SPI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);		//使能APB2外设时钟

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;			//SPI主
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//数据格式8位
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;				//时钟极性低,高都可以
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;		    //时钟相位第1,2边沿触发都可以
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler	= SPI_BaudRatePrescaler_32; //必须是16分频
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;		//MSB在前,必须
	SPI_InitStructure.SPI_CRCPolynomial = 7;				//CRC多项式校验,可有可无
	SPI_Init(SPI1,&SPI_InitStructure);

	SPI_Cmd(SPI1,ENABLE);
}
/*----------------------------------------
 函数功能: LDC1000写寄存器
 函数参数: - addr 寄存器地址
 		   - dat  数据
----------------------------------------*/
void LDC1000_WriteReg(int8_t addr, uint8_t dat)
{
	addr = addr & 0x7F;     		//MSB为0写寄存器,为1读寄存器
	CSB_0;
	
	LDC1000_SendByte(addr);
	LDC1000_SendByte(dat);
// 	SPI_I2S_SendData(SPI1, addr);
// 	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
// 	SPI_I2S_SendData(SPI1, dat);
// 	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

	CSB_1; 	
}
/*----------------------------------------
 函数功能: LDC1000读寄存器
 函数参数: - addr 寄存器地址
----------------------------------------*/
uint8_t LDC1000_ReadReg(int8_t addr)
{	
	uint8_t temp;
	addr = addr | 0x80;     		//MSB为1读寄存器,为0写寄存器
	CSB_0;
// 	SPI_I2S_SendData(SPI1, addr);
// 	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
// 	SPI_I2S_SendData(SPI1, 0x00);
// 	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
// 	temp = (uint8_t)SPI_I2S_ReceiveData(SPI1);
	
	LDC1000_SendByte(addr);
	temp = LDC1000_SendByte(0x00);
	CSB_1;
	return temp; 	
}
/*----------------------------------------
 函数功能: 向LDC1000发送一字节数据
 函数参数: 要发送的数据dat
 返 回 值: 器件发回来的数据
----------------------------------------*/
uint8_t LDC1000_SendByte(uint8_t dat)
{
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, dat);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	return (uint8_t)SPI1->DR;
}
/*----------------------------------------
 函数功能: 读取LDC1000一个字节
 函数参数: 空
 返 回 值: 从器件发来的一字节数据
----------------------------------------*/
uint8_t LDC1000_ReadByte(void)
{
	return 0x00;	
}
/*----------------------------------------
 函数功能: 读取LDC1000 len个字节
 函数参数: - addr
 		   - pBuffer
		   - len
 调用示例: LDC1000_ReadBytes(LDC1000_CMD_REVID,&RegArray[0],12);
----------------------------------------*/
void LDC1000_ReadBytes(int8_t addr, uint8_t* pBuffer, uint8_t len)
{
 	addr = addr | 0x80;		 //Read Data
	CSB_0;
	LDC1000_SendByte(addr);
	while(len > 0)
	{
	 	*pBuffer = LDC1000_ReadByte();
		pBuffer++;
		len--;	
	}
	CSB_1;
}

/********************************************************************************/
uint8_t SPI_LDC1000_ReadBytes(char ReadAddr,char* pBuffer,u8 NumByteToRead)//main()函数中读取Rp时会用到这个函数   
{ 
 	uint8_t i,readback;
	uint8_t txReadAddr;
	txReadAddr = ReadAddr | 0x80; 		  //高位是1表示读										    
	CSB_0;                            //使能器件   
    SPIx_ReadWriteByte(txReadAddr);         //发送读取命令     
//    SPIx_ReadWriteByte((u8)txReadAddr);   
    for(i=0;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=SPIx_ReadWriteByte(0XFF);   //循环读数  
    }
	CSB_1;                            //取消片选 
	return readback;    	      
}  
//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
uint8_t SPIx_ReadWriteByte(u8 TxData)
{		
	uint8_t retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
		{
		retry++;
		if(retry>200)return 0;
		}			  
	SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个数据
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); //检查指定的SPI标志位设置与否:接受缓存非空标志位
		{
		retry++;
		if(retry>200)return 0;
		}	  						    
	return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据					    
}
