#include "APP_EXTI.H"
#include "delay.h"




/**@brief 	
 * @param 	
 * @return 	
 **/
static void EXTI_NVIC_Init(void)
{
//    NVIC_InitTypeDef NVIC_InitStructure;

//    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn; //RTC全局中断
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能该通道中断
//    NVIC_Init(&NVIC_InitStructure);
}

/**@brief 	
 * @param 	
 * @return 	
 **/
static void EXTI_NVIC_Config(void)
{
    EXTI_InitTypeDef EXTI_InitStruct;
    RCC_APB2PeriphClockCmd(RCC_APB2ENR_SYSCFG, ENABLE);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1);
    EXTI_StructInit(&EXTI_InitStruct);
    EXTI_InitStruct.EXTI_Line = EXTI_Line1;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
}

/**@brief 	
 * @param 	
 * @return 	
 **/
void EXTI_Config(void)
{
	
}

void RTC_IRQHandler(void)
{
    
}
