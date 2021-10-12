//头文件调用
#include "usually.h"
#include "usart.h"


uint8_t RXflag=0;
uint16_t DataLen=0;
//uint8_t APDU[6]={0X00};
uint8_t DATA[DATALEN]={0X00};
uint16_t LenofRxData = 0;

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int Data, FILE *f)
{   
	while(!USART_GetFlagStatus(USART1,USART_FLAG_TXE));	  //USART_GetFlagStatus：得到发送状态位
														  //USART_FLAG_TXE:发送寄存器为空 1：为空；0：忙状态
	USART_SendData(USART1,Data);						  //发送一个字符
	   
	return Data;										  //返回一个值
}
#endif 

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: USART1_Send_Byte
** 功能描述: 串口发送一个字符串
** 参数描述：Data 要发送的数据
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void USART1_Send_Byte(uint16_t Data)
{ 
	while(!USART_GetFlagStatus(USART1,USART_FLAG_TXE));	  //USART_GetFlagStatus：得到发送状态位
														  //USART_FLAG_TXE:发送寄存器为空 1：为空；0：忙状态
	USART_SendData(USART1,Data);						  //发送一个字符
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: USART1_Send_Byte
** 功能描述: 串口发送一个字符串
** 参数描述：Data 要发送的数据
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
uint8_t USART1_Receive_Byte(void)
{ 
   	while(!(USART_GetFlagStatus(USART1,USART_FLAG_RXNE))); //USART_GetFlagStatus：得到接收状态位
														   //USART_FLAG_RXNE:接收数据寄存器非空标志位 
														   //1：忙状态  0：空闲(没收到数据，等待。。。)
	return USART_ReceiveData(USART1);					   //接收一个字符
}

void Usart_DmaConfig(void){
	DMA_InitTypeDef dmaTypeStruct;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	DMA_DeInit(DMA1_Channel5);
	/*!< Specifies the peripheral base address for DMAy Channelx. */
	dmaTypeStruct.DMA_PeripheralBaseAddr=(uint32_t)(&USART1->DR);
	dmaTypeStruct.DMA_MemoryBaseAddr=(uint32_t)DATA;
	dmaTypeStruct.DMA_DIR=DMA_DIR_PeripheralSRC;
	dmaTypeStruct.DMA_BufferSize=DATALEN;
	dmaTypeStruct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	dmaTypeStruct.DMA_MemoryInc=DMA_MemoryInc_Enable;
	dmaTypeStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
	dmaTypeStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;
	dmaTypeStruct.DMA_Mode=DMA_Mode_Circular;
	dmaTypeStruct.DMA_Priority = DMA_Priority_High;
	dmaTypeStruct.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel5,&dmaTypeStruct);
	
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);
	//DMA_GetFlagStatus(DMA2_FLAG_TC3);
	//DMA_ClearFlag(DMA1_FLAG_TC5);
	//DMA_ClearITPendingBit(DMA1_IT_TC5);
	DMA_ITConfig(DMA1_Channel5,DMA_IT_TC,ENABLE);
	DMA_ITConfig(DMA1_Channel5,DMA_IT_TE,ENABLE);
	DMA_Cmd(DMA1_Channel5,ENABLE);
}

/************************************************************
 * 函数名称: Usart_Init
 * 功能描述: 串口引脚初始化
 * 参数描述: 无
 ************************************************************/
void UsartGPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;					//定义一个GPIO结构体变量
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,ENABLE);	
															//使能各个端口时钟，重要！！！
	// PA9 USART1 Tx
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 			//配置串口接收端口挂接到9端口
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   		//复用功能输出开漏
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	//配置端口速度为50M
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				   	//根据参数初始化GPIOA寄存器	
	
	// PA10 USART1 Rx
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入(复位状态);	   				
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				   	//根据参数初始化GPIOA寄存器	
}
/************************************************************
 * 函数名称: Usart_Configuration
 * 功能描述: 串口配置函数
 * 参数描述: BaudRate设置波特率 
 ************************************************************/
void Init_Usart(uint32_t BaudRate)
{
	USART_InitTypeDef USART_InitStructure;							    	//定义一个串口结构体
	
	UsartGPIO_Config();
	USART_InitStructure.USART_BaudRate            =BaudRate ;	  			//波特率115200
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b; 	//传输过程中使用8位数据
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;	 	//在帧结尾传输1位停止位
	USART_InitStructure.USART_Parity              = USART_Parity_No ;	 	//奇偶失能
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流失能
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx; //接收和发送模式
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART1, &USART_InitStructure);								//根据参数初始化串口寄存器
	USART_ClearFlag(USART1, USART_FLAG_TC);
	USART_ITConfig(USART1,USART_IT_TC,DISABLE);
	USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);							//使能串口中断接收
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);
	USART_Cmd(USART1, ENABLE);     											//使能串口外设
	
	Usart_DmaConfig();
	
}

/**************************************
 * 功能：发送一串字节数据
 * pbuff：指向待发送字节数据地址 
 * len  ：指向待发送数据长度
 ***************************************/
void USART1_SendBuff(u8 *pbuff,u32 len)
{
	u32 i;
	for(i=0;i<len;i++)
	{
		USART_SendData(USART1,*pbuff);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
		pbuff++;
	}
}

/**************************************
 * 功能：发送一串字符串数据
 * str：指向待发送字符串数据地址
 ***************************************/
void USART1_SendString(char const *str)
{
	//USART_SendData (USART1,*str);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	while (1) 
	{
		if (*str == '\0') 
		{
			break;                                                      /* 遇到结束符，退出             */
		}
		USART_SendData (USART1,*str++);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	}	
}

/**************************************
 * 功能：发送字符串数据加字节数据，全部以字符串数据形式发送，用于调试
 * str ： 指向待发送字符串数据地址
 *pbuff：指向待发送字节数据地址
 *len  ：指向待发送字节数据长度 
 ***************************************/
void USART1_Debug(char const *str,u8 *pbuff,u32 len)
{
	u16 i;
	char sbuf[2];
	
	USART1_SendString(str);  
    for(i=0;i<len;i++)
	{// sprintf将u8类型的数据转换成字符串，例如0x9a就变成了"9a"
		sprintf((char*)sbuf,"%02X ",(uint32_t)pbuff[i]);
		USART1_SendBuff((uint8_t *)sbuf, 3);
	}
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: USART1_Send_Byte
** 功能描述: 串口发送一个字符串
** 参数描述：Data 要发送的数据
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void USART_Send_Byte(USART_TypeDef* USARTx, uint16_t Data)
{ 
	while(!USART_GetFlagStatus(USARTx,USART_FLAG_TXE));	  //USART_GetFlagStatus：得到发送状态位
														  //USART_FLAG_TXE:发送寄存器为空 1：为空；0：忙状态
	USART_SendData(USARTx, Data);						  //发送一个字符
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: USART1_Send_Byte
** 功能描述: 串口发送一个字符串
** 参数描述：Data 要发送的数据
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
uint8_t USART_Receive_Byte(USART_TypeDef* USARTx)
{ 
	
   	while(!(USART_GetFlagStatus(USARTx,USART_FLAG_RXNE))); //USART_GetFlagStatus：得到接收状态位
														   //USART_FLAG_RXNE:接收数据寄存器非空标志位 
														   //1：忙状态  0：空闲(没收到数据，等待。。。)
	return USART_ReceiveData(USARTx);					   //接收一个字符
}

void Delay_Ms1(uint16_t time)  //延时函数
{ 
	uint16_t i,j;
	for(i=0;i<time;i++)
  		for(j=0;j<10260;j++);
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: USART1_IRQHandler
** 功能描述: 串口中断函数
** 参数描述: 无 
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void USART1_IRQHandler()
{
	uint8_t TmpReg;

	// 空闲总线中断触发到来
	if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)		
	{
		// 关闭DMA,防止处理其间有数据
		DMA_Cmd(DMA1_Channel5, DISABLE);						
		// 递减计数
		LenofRxData = DATALEN - DMA_GetCurrDataCounter(DMA1_Channel5);
		DataLen = LenofRxData;

		// 重装待接收数据的长度
		DMA_SetCurrDataCounter(DMA1_Channel5, DATALEN);   
		DMA_Cmd(DMA1_Channel5, ENABLE);
		
		 // 先读SR，然后读DR以清除空闲中断状态
		TmpReg += USART1->SR;			   		   				   
    	TmpReg += USART1->DR;
		TmpReg = TmpReg;
		RXflag = 1;
	}
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
//		Res =USART_Receive_Byte(USART1);
	}
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
End:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
