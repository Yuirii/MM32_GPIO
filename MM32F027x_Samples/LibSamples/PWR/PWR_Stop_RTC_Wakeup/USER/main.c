////////////////////////////////////////////////////////////////////////////////
/// @file    main.c
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
#define _MAIN_C_

// Files includes


#include "led.h"
#include "pwr.h"
#include "uart.h"
#include "tim1_interrupt.h"
#include "delay.h"
#include "sys.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Example_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MAIN
/// @{
#define LPWR	0
////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MAIN_Exported_Constants
/// @{
extern u8 timeflag;
extern u32 SystemCoreClock;
extern u8 count;


////////////////////////////////////////////////////////////////////////////////
/// @brief  This function is main entrance.
/// @param  None.
/// @retval  0.
////////////////////////////////////////////////////////////////////////////////
void Init_NVIC(void)
{ 	
  	exNVIC_Init_TypeDef NVIC_InitStructure;			//定义一个NVIC向量表结构体变量

	NVIC_InitStructure.NVIC_IRQChannel = UART1_IRQn;			//配置串口1为中断源
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 	//设置占先优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  	//设置副优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  	//使能串口1中断
	exNVIC_Init(&NVIC_InitStructure);							  	//根据参数初始化中断寄存器
	
	// 配置DMA通道15
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	exNVIC_Init(&NVIC_InitStructure);
}


s32 main(void)
{
	SystemReInit(3);//11 6
    LED_Init();
	
	CONSOLE_Init(115200);
	Init_NVIC();
//    PWR_STOP_RTC_Init();
//	RJPrintInfo("\n\r Init Complete ... ", 0, 0);
	USART1_SendString("2222\n");
	printf("init.");
    while(1) {
		
		
#if LPWR
		LED_DeInit();
		LED_Init();
		CONSOLE_Init(115200);
		LED_DeInit();
		PWR_STOP_RTC_Init();
        RTC_SetCounter(0);
        PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
#endif
	}

}



/// @}

/// @}

/// @}
