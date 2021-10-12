#ifndef __USART_H
#define __USART_H

//头文件包含
#include "stdio.h"
#include <stdint.h>
#include <string.h>
#include <stm32f10x.h>

#define DEBUG_OUTPUT 1

#if DEBUG_OUTPUT
#define RJPrintInfo(fmt, a...) USART1_Debug(fmt, ##a)
#else
#define RJPrintInfo(fmt, a...) ((void)0)
#endif

#define DATALEN 1280

extern uint8_t RXflag;
extern uint8_t CRC8_Data;
extern uint8_t Lenbuff[4];

extern uint16_t DataLen;
extern uint8_t DATA[DATALEN];
extern uint16_t LenofRxData;

//函数声明
void USART1_Send_Byte(uint16_t dat);
uint8_t USART1_Receive_Byte(void);
uint8_t USART_Receive_Byte(USART_TypeDef* USARTx);
void UsartGPIO_Config(void);
void Init_Usart(uint32_t BaudRate); 
extern void USART1_Debug(char const *str,u8 *pbuff,u32 len);


#endif

