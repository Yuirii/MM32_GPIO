/****************************************************************************
*			    Copyright 2017 by WuHan Runjetic, All rights reserved.
*****************************************************************************
*@Filename :  platform.h
*@Author   :  Yuwei
*@Date     :  2018-12-27
*@Brief    :  数据处理文件
*****************************************************************************
*
*/

#ifndef __PLATFORM_H
#define __PLATFORM_H

//头文件调用

#include "hardware.h"

void Delay(u32 nTime);

// RJGT401S复位控制引脚宏定义
#define GPIO_Port_Reset             GPIOB
#define GPIO_Pin_Reset        		GPIO_Pin_11
#define RJGT401S_RESET							if(1)\
	do											\
	{											\
		GPIO_Port_Reset->BSRR = GPIO_Pin_Reset;	\
		Delay(1000);							\
		GPIO_Port_Reset->BRR = GPIO_Pin_Reset;	\
		Delay(1000);							\
	} while(0)

//函数申明
extern void Init_LED(void);	
extern void Init_NVIC(void);
extern void Init_TIMER(void);
extern void Init_RstGPIO(void);

#endif

