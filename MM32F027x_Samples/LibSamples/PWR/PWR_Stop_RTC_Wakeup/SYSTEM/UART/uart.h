////////////////////////////////////////////////////////////////////////////////
/// @file    uart.h
/// @author  AE TEAM
/// @brief   THIS FILE PROVIDES ALL THE SYSTEM FIRMWARE FUNCTIONS.
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
#ifndef __UART_H
#define __UART_H
// Files includes
#include "mm32_device.h"
#include  "stdio.h"


////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_Example_Layer
/// @brief MM32 Example Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_RESOURCE
/// @brief MM32 Examples resource modules
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_Exported_Constants
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_Exported_Variables
/// @{
#ifdef _UART_C_
#define GLOBAL






#else
#define GLOBAL extern






#endif
#define USART_DMAReq_Tx                      ((uint16_t)0x0080)
#define USART_DMAReq_Rx                      ((uint16_t)0x0040)
#define IS_USART_DMAREQ(DMAREQ) ((((DMAREQ) & (uint16_t)0xFF3F) == 0x00) && ((DMAREQ) != (uint16_t)0x00))

#define USART_FLAG_CTS                       ((uint16_t)0x0200)	//CST标志位
#define USART_FLAG_LBD                       ((uint16_t)0x0100)	//LIN中断检测标志位
#define USART_FLAG_TXE                       ((uint16_t)0x0080)	//发送寄存器为空
#define USART_FLAG_TC                        ((uint16_t)0x0040)	//发送完标志位
#define USART_FLAG_RXNE                      ((uint16_t)0x0020)	//接收数据寄存器非空标志位
#define USART_FLAG_IDLE                      ((uint16_t)0x0010)	//空闲总线标志位
#define USART_FLAG_ORE                       ((uint16_t)0x0008)	//溢出错误标志位
#define USART_FLAG_NE                        ((uint16_t)0x0004)	//噪音错误标志位
#define USART_FLAG_FE                        ((uint16_t)0x0002)	//帧错误标志位
#define USART_FLAG_PE                        ((uint16_t)0x0001)	//奇偶错误标志位

#define USART_IT_PE                          ((uint16_t)0x0028)
#define USART_IT_TXE                         ((uint16_t)0x0727)
#define USART_IT_TC                          ((uint16_t)0x0626)
#define USART_IT_RXNE                        ((uint16_t)0x0525)
#define USART_IT_IDLE                        ((uint16_t)0x0424)
#define USART_IT_LBD                         ((uint16_t)0x0846)
#define USART_IT_CTS                         ((uint16_t)0x096A)
#define USART_IT_ERR                         ((uint16_t)0x0060)
#define USART_IT_ORE                         ((uint16_t)0x0360)
#define USART_IT_NE                          ((uint16_t)0x0260)
#define USART_IT_FE                          ((uint16_t)0x0160)

#define RJPrintInfo(fmt, a...) USART1_Debug(fmt, ##a)
#define USART_FLAG_CTS                       ((uint16_t)0x0200)	//CST标志位
#define USART_FLAG_LBD                       ((uint16_t)0x0100)	//LIN中断检测标志位
#define USART_FLAG_TXE                       ((uint16_t)0x0080)	//发送寄存器为空
#define USART_FLAG_TC                        ((uint16_t)0x0040)	//发送完标志位
#define USART_FLAG_RXNE                      ((uint16_t)0x0020)	//接收数据寄存器非空标志位
#define USART_FLAG_IDLE                      ((uint16_t)0x0010)	//空闲总线标志位
#define USART_FLAG_ORE                       ((uint16_t)0x0008)	//溢出错误标志位
#define USART_FLAG_NE                        ((uint16_t)0x0004)	//噪音错误标志位
#define USART_FLAG_FE                        ((uint16_t)0x0002)	//帧错误标志位
#define USART_FLAG_PE                        ((uint16_t)0x0001)	//奇偶错误标志位
#define DATALEN 1280

extern uint8_t RXflag;
extern uint8_t CRC8_Data;
extern uint8_t Lenbuff[4];

extern uint16_t DataLen;
extern uint8_t DATA[DATALEN];
extern uint16_t LenofRxData;


#undef GLOBAL

/// @}


////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_Exported_Functions
/// @{

void CONSOLE_Init(u32 baudrate);
void USART1_SendBuff(u8 *pbuff,u32 len);
void USART1_SendString(char const *str);
void USART1_Debug(char const *str,u8 *pbuff,u32 len);

/// @}


/// @}

/// @}


////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////


