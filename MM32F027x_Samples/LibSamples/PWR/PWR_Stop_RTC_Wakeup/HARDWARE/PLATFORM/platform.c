/****************************************************************************
*			    Copyright 2018 by WuHan Runjetic, All rights reserved.
*****************************************************************************
*@Filename :  platform.c
*@Author   :  Yuwei
*@Date     :  2018-12-27
*@Brief    :  数据处理文件
*****************************************************************************
*
*/

#include "platform.h"
#include "usually.h"

void Delay(u32 nTime)
{
	int rTimingDelay = nTime,i;
	
	while(rTimingDelay != 0){
		for(i=0;i<1000; i++)
			rTimingDelay--;
	}
}

void Init_RstGPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void Init_LED(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;					//定义一个GPIO结构体变量
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOG | RCC_APB2Periph_GPIOD, ENABLE);	//使能各个端口时钟，重要！！！
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);	//使能各个端口时钟，重要！！！
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;				//配置LED端口挂接到14端口
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   	//通用输出推挽
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	//配置端口速度为50M
  	GPIO_Init(GPIOG, &GPIO_InitStructure);				   	//根据参数初始化GPIOD寄存器
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				//配置LED端口13
  	GPIO_Init(GPIOD, &GPIO_InitStructure);				   	//根据参数初始化GPIOD寄存器
	LED1 = 1;
	LED2 = 0;
}

/**************************************************************
 * 函数名称: TIMER_Init
 * 功能描述: 定时器2初始化配置
 * 参数描述：无
 **************************************************************/
void Init_TIMER(void)
{
	TIM_TimeBaseInitTypeDef	 TIM_BaseInitStructure;			//定义一个定时器结构体变量

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);   	//使能定时器2 <时钟>，重要！！
	TIM_DeInit(TIM2);                              			//将IM2定时器初始化位复位值
	TIM_InternalClockConfig(TIM2); 							//配置 TIM2 内部时钟
	TIM_BaseInitStructure.TIM_Period = 5000; 				//设置自动重载寄存器值为最大值	0~65535之间  1000000/1000=1000us=1ms													
															//TIM_Period（TIM1_ARR）=1000，计数器向上计数到1000后产生更新事件，
															//计数值归零 也就是 1MS产生更新事件一次
	TIM_BaseInitStructure.TIM_Prescaler = 7200-1;  			//自定义预分频系数，提供给定时器的时钟	0~65535之间
															//设置预分频器分频系数7200-1，即APB2=72M, TIM1_CLK=72/72=10KHz 
	TIM_BaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; //时钟分割为0
	TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;   
															//TIM向上计数模式 从0开始向上计数，计数到1000后产生更新事件
//	TIM_BaseInitStructure.TIM_RepetitionCounter = 0; 		//此参数只适合于TIM1和TIM8
	TIM_TimeBaseInit(TIM2, &TIM_BaseInitStructure); 		//根据指定参数初始化TIM时间基数寄存器
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update); 					//清中断，以免一启用中断后立即产生中断 
    //TIM_ClearFlag(TIM2, TIM_FLAG_Break);
    TIM_ARRPreloadConfig(TIM2, DISABLE);					//禁止ARR 预装载缓冲器

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); 				//使能TIM2中断源 
	TIM_Cmd(TIM2, DISABLE); 								//TIM2总开关：开启 
}

void Init_NVIC(void)
{ 	
  	NVIC_InitTypeDef NVIC_InitStructure;			//定义一个NVIC向量表结构体变量

	#ifdef VECT_TAB_RAM  							//向量表基地址选择
	  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);  	//将0x20000000地址作为向量表基地址(RAM)
	#else
	  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0); //将0x08000000地址作为向量表基地址(FLASH)  
	#endif

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置中断组 为2 

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;			//配置串口1为中断源
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 	//设置占先优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  	//设置副优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  	//使能串口1中断
	NVIC_Init(&NVIC_InitStructure);							  	//根据参数初始化中断寄存器
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;			//配置串口1为中断源
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 	//设置占先优先级为2
	NVIC_Init(&NVIC_InitStructure);	
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; 			//中断事件为TIM2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级0   
	NVIC_Init(&NVIC_InitStructure);                         	//根据指定参数初始化中断寄存器
	
	// 配置DMA通道15
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

