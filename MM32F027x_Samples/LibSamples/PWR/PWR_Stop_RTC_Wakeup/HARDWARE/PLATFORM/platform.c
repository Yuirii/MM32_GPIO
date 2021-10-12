/****************************************************************************
*			    Copyright 2018 by WuHan Runjetic, All rights reserved.
*****************************************************************************
*@Filename :  platform.c
*@Author   :  Yuwei
*@Date     :  2018-12-27
*@Brief    :  ���ݴ����ļ�
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
	GPIO_InitTypeDef GPIO_InitStructure;					//����һ��GPIO�ṹ�����
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOG | RCC_APB2Periph_GPIOD, ENABLE);	//ʹ�ܸ����˿�ʱ�ӣ���Ҫ������
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);	//ʹ�ܸ����˿�ʱ�ӣ���Ҫ������
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;				//����LED�˿ڹҽӵ�14�˿�
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   	//ͨ���������
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	//���ö˿��ٶ�Ϊ50M
  	GPIO_Init(GPIOG, &GPIO_InitStructure);				   	//���ݲ�����ʼ��GPIOD�Ĵ���
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				//����LED�˿�13
  	GPIO_Init(GPIOD, &GPIO_InitStructure);				   	//���ݲ�����ʼ��GPIOD�Ĵ���
	LED1 = 1;
	LED2 = 0;
}

/**************************************************************
 * ��������: TIMER_Init
 * ��������: ��ʱ��2��ʼ������
 * ������������
 **************************************************************/
void Init_TIMER(void)
{
	TIM_TimeBaseInitTypeDef	 TIM_BaseInitStructure;			//����һ����ʱ���ṹ�����

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);   	//ʹ�ܶ�ʱ��2 <ʱ��>����Ҫ����
	TIM_DeInit(TIM2);                              			//��IM2��ʱ����ʼ��λ��λֵ
	TIM_InternalClockConfig(TIM2); 							//���� TIM2 �ڲ�ʱ��
	TIM_BaseInitStructure.TIM_Period = 5000; 				//�����Զ����ؼĴ���ֵΪ���ֵ	0~65535֮��  1000000/1000=1000us=1ms													
															//TIM_Period��TIM1_ARR��=1000�����������ϼ�����1000����������¼���
															//����ֵ���� Ҳ���� 1MS���������¼�һ��
	TIM_BaseInitStructure.TIM_Prescaler = 7200-1;  			//�Զ���Ԥ��Ƶϵ�����ṩ����ʱ����ʱ��	0~65535֮��
															//����Ԥ��Ƶ����Ƶϵ��7200-1����APB2=72M, TIM1_CLK=72/72=10KHz 
	TIM_BaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; //ʱ�ӷָ�Ϊ0
	TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;   
															//TIM���ϼ���ģʽ ��0��ʼ���ϼ�����������1000����������¼�
//	TIM_BaseInitStructure.TIM_RepetitionCounter = 0; 		//�˲���ֻ�ʺ���TIM1��TIM8
	TIM_TimeBaseInit(TIM2, &TIM_BaseInitStructure); 		//����ָ��������ʼ��TIMʱ������Ĵ���
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update); 					//���жϣ�����һ�����жϺ����������ж� 
    //TIM_ClearFlag(TIM2, TIM_FLAG_Break);
    TIM_ARRPreloadConfig(TIM2, DISABLE);					//��ֹARR Ԥװ�ػ�����

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); 				//ʹ��TIM2�ж�Դ 
	TIM_Cmd(TIM2, DISABLE); 								//TIM2�ܿ��أ����� 
}

void Init_NVIC(void)
{ 	
  	NVIC_InitTypeDef NVIC_InitStructure;			//����һ��NVIC������ṹ�����

	#ifdef VECT_TAB_RAM  							//���������ַѡ��
	  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);  	//��0x20000000��ַ��Ϊ���������ַ(RAM)
	#else
	  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0); //��0x08000000��ַ��Ϊ���������ַ(FLASH)  
	#endif

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�����ж��� Ϊ2 

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;			//���ô���1Ϊ�ж�Դ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 	//����ռ�����ȼ�Ϊ2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  	//���ø����ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  	//ʹ�ܴ���1�ж�
	NVIC_Init(&NVIC_InitStructure);							  	//���ݲ�����ʼ���жϼĴ���
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;			//���ô���1Ϊ�ж�Դ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 	//����ռ�����ȼ�Ϊ2
	NVIC_Init(&NVIC_InitStructure);	
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; 			//�ж��¼�ΪTIM2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�0   
	NVIC_Init(&NVIC_InitStructure);                         	//����ָ��������ʼ���жϼĴ���
	
	// ����DMAͨ��15
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

