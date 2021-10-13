////////////////////////////////////////////////////////////////////////////////
/// @file    uart.c
/// @author  AE TEAM
/// @brief   THIS FILE PROVIDES ALL THE SYSTEM FUNCTIONS.
////////////////////////////////////////////////////////////////////////////////
/// @attention
///
/// THE EXISTING FIRMWARE IS ONLY FOR REFERENCE, WHICH IS DESIGNED TO PROVIDE
/// CUSTOMERS WITH CODING INFORMATION ABOUT THEIR PRODUCTS SO THEY CAN SAVE
/// TIME. THEREFORE, MINDMOTION SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT OR
/// CONSEQUENTIAL DAMAGES ABOUT ANY CLAIMS ARISING OUT OF THE CONTENT OF SUCH
/// HARDWARE AND/OR THE USE OF THE CODING INFORMATION CONTAINED HEREIN IN
/// CONNECTION WITH PRODUCTS MADE BY CUSTOMERS.
///
/// <H2><CENTER>&COPY; COPYRIGHT MINDMOTION </CENTER></H2>
////////////////////////////////////////////////////////////////////////////////

// Define to prevent recursive inclusion
#define _UART_C_

// Files includes
#include <stdio.h>
#include "uart.h"
#include "hal_conf.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Hardware_Abstract_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup UART
/// @{
uint8_t RXflag=0;
uint16_t DataLen=0;
//uint8_t APDU[6]={0X00};
uint8_t DATA[DATALEN]={0X00};
uint16_t LenofRxData = 0;
////////////////////////////////////////////////////////////////////////////////
/// @addtogroup UART_Exported_Functions
/// @{

#ifdef __GNUC__

#define PUTCHAR_PROTOTYPE s32 __io_putchar(s32 ch)
#else
#define PUTCHAR_PROTOTYPE s32 fputc(s32 ch, FILE *f)

#endif

#ifdef USE_IAR
PUTCHAR_PROTOTYPE {
    while((UART1->CSR & UART_IT_TXIEN) == 0); //The loop is sent until it is finished
    UART1->TDR = (ch & (u16)0x00FF);
    return ch;
}

#else
void _sys_exit(s32 x)
{
    x = x;
}
//redefine fputcfunction
s32 fputc(s32 ch, FILE* f)
{
    while((UART1->CSR & UART_IT_TXIEN) == 0); //The loop is sent until it is finished
    UART1->TDR = (ch & (u16)0x00FF);
    return ch;
}

#endif


/**@brief 	
 * @param 	
 * @return 	
 **/
void Usart_DmaConfig(void){
	DMA_InitTypeDef dmaTypeStruct;
	RCC_AHBPeriphClockCmd(RCC_AHBENR_DMA1,ENABLE);
	
	DMA_DeInit(DMA1_Channel3);
	/*!< Specifies the peripheral base address for DMAy Channelx. */
	dmaTypeStruct.DMA_PeripheralBaseAddr=(uint32_t)(&UART1->RDR);//(uint32_t)(&UART1->TDR) | (uint32_t)(&UART1->RDR);
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
	DMA_Init(DMA1_Channel3,&dmaTypeStruct);
	__NOP();
	__NOP();
	
	UART_DMACmd(UART1,USART_DMAReq_Rx,ENABLE);
	__NOP();
	__NOP();
	//DMA_GetFlagStatus(DMA2_FLAG_TC3);
	//DMA_ClearFlag(DMA1_FLAG_TC5);
	//DMA_ClearITPendingBit(DMA1_IT_TC5);
	DMA_ITConfig(DMA1_Channel3,DMA_IT_TC,ENABLE);
	DMA_ITConfig(DMA1_Channel3,DMA_IT_TE,ENABLE);
	DMA_Cmd(DMA1_Channel3,ENABLE);
	__NOP();
	__NOP();
}

/**@brief 	send buff.
 * @param 	
 * @return 	
 **/
void USART1_SendBuff(u8 *pbuff,u32 len)
{
	u32 i;
	for(i=0;i<len;i++)
	{
		UART_SendData(UART1,*pbuff);
		while(UART_GetFlagStatus(UART1,UART_FLAG_TXEPT)!=SET);
		pbuff++;
	}
}

/**@brief 	send string.
 * @param 	
 * @return 	
 **/
void USART1_SendString(char const *str)
{
	//USART_SendData (USART1,*str);
	while(UART_GetFlagStatus(UART1,UART_FLAG_TXEPT)!=SET);
	while (1) 
	{
		if (*str == '\0') 
		{
			break;                                                      /* 遇到结束符，退出             */
		}
		UART_SendData (UART1,*str++);
		while(UART_GetFlagStatus(UART1,UART_FLAG_TXEPT)!=SET);
	}	
}

/**@brief 	ASCII+HEX
 * @param 	
 * @return 	
 **/
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

/**@brief 	
 * @param 	
 * @return 	
 **/
void UART1_IRQHandler()
{
	uint8_t TmpReg;

	// 空闲总线中断触发到来
	if (UART_GetITStatus(UART1, UART_IER_RXIDLE) != RESET)		
	{
		// 关闭DMA,防止处理其间有数据
		DMA_Cmd(DMA1_Channel3, DISABLE);						
		// 递减计数
		LenofRxData = DATALEN - DMA_GetCurrDataCounter(DMA1_Channel3);
		DataLen = LenofRxData;

		// 重装待接收数据的长度
		DMA_SetCurrDataCounter(DMA1_Channel3, DATALEN);   
		DMA_Cmd(DMA1_Channel3, ENABLE);
		
		 // 先读SR，然后读DR以清除空闲中断状态
		TmpReg += UART1->CSR;
		TmpReg += UART1->ISR;		
    	TmpReg += UART1->TDR;
		TmpReg += UART1->RDR;
		TmpReg = TmpReg;
		RXflag = 1;
	}
	
	if(UART_GetITStatus(UART1, UART_IT_RXIEN) != RESET)
	{
//		Res =USART_Receive_Byte(USART1);
	}
}

void CONSOLE_Init(u32 baudrate)
{
    //GPIO port set
    GPIO_InitTypeDef GPIO_InitStruct;
    UART_InitTypeDef UART_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2ENR_UART1, ENABLE);   //enableUART1,GPIOAclock
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOA, ENABLE);  //
    //UART initialset
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);



    UART_StructInit(&UART_InitStruct);
    UART_InitStruct.UART_BaudRate = baudrate;
    UART_InitStruct.UART_WordLength = UART_WordLength_8b;
    UART_InitStruct.UART_StopBits = UART_StopBits_1;//one stopbit
    UART_InitStruct.UART_Parity = UART_Parity_No;//none odd-even  verify bit
    UART_InitStruct.UART_HardwareFlowControl = UART_HardwareFlowControl_None;//No hardware flow control
    UART_InitStruct.UART_Mode = UART_Mode_Rx | UART_Mode_Tx; // receive and sent  mode

    UART_Init(UART1, &UART_InitStruct); //initial uart 1
	UART_ClearITPendingBit(UART1, USART_FLAG_TC);
	UART_ITConfig(UART1,UART_IT_TXIEN,DISABLE);
	UART_ITConfig(UART1,UART_IT_RXIEN,DISABLE);							//使能串口中断接收
	UART_ITConfig(UART1,UART_IER_RXIDLE,ENABLE);
    UART_Cmd(UART1, ENABLE);                    //enable uart 1

    //UART1_TX   GPIOA.9
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    //UART1_RX    GPIOA.10
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	
	
	Usart_DmaConfig();

}



/// @}

/// @}

/// @}
