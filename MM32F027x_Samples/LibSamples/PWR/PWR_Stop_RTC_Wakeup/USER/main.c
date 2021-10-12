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
s32 main(void)
{
	SystemReInit(11);//11 6
    LED_Init();
	CONSOLE_Init(115200);
//	NVIC_Configure(TIM1_BRK_UP_TRG_COM_IRQn, 3, 1);
//	TIM1_Init(4, 60);//HSI = 4MHz , psc = 60 => perid = 0 >> 
    PWR_STOP_RTC_Init();
    while(1) {
//		LED1_TOGGLE();

		LED_DeInit();
		LED_Init();
		CONSOLE_Init(115200);
//		NVIC_Configure(TIM1_BRK_UP_TRG_COM_IRQn, 3, 1);
//		TIM1_Init(4, 60);
		LED_DeInit();
//		deleyNop(10);
//		printf("wkup");
		PWR_STOP_RTC_Init();
//		NVIC_DeConfigure(TIM1_BRK_UP_TRG_COM_IRQn, 3, 1);
        RTC_SetCounter(0);
        PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
    
	}

}



/// @}

/// @}

/// @}
