//ͷ�ļ�����
#include "usually.h"
#include "usart.h"


uint8_t RXflag=0;
uint16_t DataLen=0;
//uint8_t APDU[6]={0X00};
uint8_t DATA[DATALEN]={0X00};
uint16_t LenofRxData = 0;

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int Data, FILE *f)
{   
	while(!USART_GetFlagStatus(USART1,USART_FLAG_TXE));	  //USART_GetFlagStatus���õ�����״̬λ
														  //USART_FLAG_TXE:���ͼĴ���Ϊ�� 1��Ϊ�գ�0��æ״̬
	USART_SendData(USART1,Data);						  //����һ���ַ�
	   
	return Data;										  //����һ��ֵ
}
#endif 

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** ��������: USART1_Send_Byte
** ��������: ���ڷ���һ���ַ���
** ����������Data Ҫ���͵�����
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void USART1_Send_Byte(uint16_t Data)
{ 
	while(!USART_GetFlagStatus(USART1,USART_FLAG_TXE));	  //USART_GetFlagStatus���õ�����״̬λ
														  //USART_FLAG_TXE:���ͼĴ���Ϊ�� 1��Ϊ�գ�0��æ״̬
	USART_SendData(USART1,Data);						  //����һ���ַ�
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** ��������: USART1_Send_Byte
** ��������: ���ڷ���һ���ַ���
** ����������Data Ҫ���͵�����
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
uint8_t USART1_Receive_Byte(void)
{ 
   	while(!(USART_GetFlagStatus(USART1,USART_FLAG_RXNE))); //USART_GetFlagStatus���õ�����״̬λ
														   //USART_FLAG_RXNE:�������ݼĴ����ǿձ�־λ 
														   //1��æ״̬  0������(û�յ����ݣ��ȴ�������)
	return USART_ReceiveData(USART1);					   //����һ���ַ�
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
 * ��������: Usart_Init
 * ��������: �������ų�ʼ��
 * ��������: ��
 ************************************************************/
void UsartGPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;					//����һ��GPIO�ṹ�����
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,ENABLE);	
															//ʹ�ܸ����˿�ʱ�ӣ���Ҫ������
	// PA9 USART1 Tx
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 			//���ô��ڽ��ն˿ڹҽӵ�9�˿�
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   		//���ù��������©
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	//���ö˿��ٶ�Ϊ50M
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				   	//���ݲ�����ʼ��GPIOA�Ĵ���	
	
	// PA10 USART1 Rx
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//��������(��λ״̬);	   				
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				   	//���ݲ�����ʼ��GPIOA�Ĵ���	
}
/************************************************************
 * ��������: Usart_Configuration
 * ��������: �������ú���
 * ��������: BaudRate���ò����� 
 ************************************************************/
void Init_Usart(uint32_t BaudRate)
{
	USART_InitTypeDef USART_InitStructure;							    	//����һ�����ڽṹ��
	
	UsartGPIO_Config();
	USART_InitStructure.USART_BaudRate            =BaudRate ;	  			//������115200
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b; 	//���������ʹ��8λ����
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;	 	//��֡��β����1λֹͣλ
	USART_InitStructure.USART_Parity              = USART_Parity_No ;	 	//��żʧ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//Ӳ����ʧ��
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx; //���պͷ���ģʽ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART1, &USART_InitStructure);								//���ݲ�����ʼ�����ڼĴ���
	USART_ClearFlag(USART1, USART_FLAG_TC);
	USART_ITConfig(USART1,USART_IT_TC,DISABLE);
	USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);							//ʹ�ܴ����жϽ���
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);
	USART_Cmd(USART1, ENABLE);     											//ʹ�ܴ�������
	
	Usart_DmaConfig();
	
}

/**************************************
 * ���ܣ�����һ���ֽ�����
 * pbuff��ָ��������ֽ����ݵ�ַ 
 * len  ��ָ����������ݳ���
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
 * ���ܣ�����һ���ַ�������
 * str��ָ��������ַ������ݵ�ַ
 ***************************************/
void USART1_SendString(char const *str)
{
	//USART_SendData (USART1,*str);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	while (1) 
	{
		if (*str == '\0') 
		{
			break;                                                      /* �������������˳�             */
		}
		USART_SendData (USART1,*str++);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	}	
}

/**************************************
 * ���ܣ������ַ������ݼ��ֽ����ݣ�ȫ�����ַ���������ʽ���ͣ����ڵ���
 * str �� ָ��������ַ������ݵ�ַ
 *pbuff��ָ��������ֽ����ݵ�ַ
 *len  ��ָ��������ֽ����ݳ��� 
 ***************************************/
void USART1_Debug(char const *str,u8 *pbuff,u32 len)
{
	u16 i;
	char sbuf[2];
	
	USART1_SendString(str);  
    for(i=0;i<len;i++)
	{// sprintf��u8���͵�����ת�����ַ���������0x9a�ͱ����"9a"
		sprintf((char*)sbuf,"%02X ",(uint32_t)pbuff[i]);
		USART1_SendBuff((uint8_t *)sbuf, 3);
	}
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** ��������: USART1_Send_Byte
** ��������: ���ڷ���һ���ַ���
** ����������Data Ҫ���͵�����
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void USART_Send_Byte(USART_TypeDef* USARTx, uint16_t Data)
{ 
	while(!USART_GetFlagStatus(USARTx,USART_FLAG_TXE));	  //USART_GetFlagStatus���õ�����״̬λ
														  //USART_FLAG_TXE:���ͼĴ���Ϊ�� 1��Ϊ�գ�0��æ״̬
	USART_SendData(USARTx, Data);						  //����һ���ַ�
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** ��������: USART1_Send_Byte
** ��������: ���ڷ���һ���ַ���
** ����������Data Ҫ���͵�����
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
uint8_t USART_Receive_Byte(USART_TypeDef* USARTx)
{ 
	
   	while(!(USART_GetFlagStatus(USARTx,USART_FLAG_RXNE))); //USART_GetFlagStatus���õ�����״̬λ
														   //USART_FLAG_RXNE:�������ݼĴ����ǿձ�־λ 
														   //1��æ״̬  0������(û�յ����ݣ��ȴ�������)
	return USART_ReceiveData(USARTx);					   //����һ���ַ�
}

void Delay_Ms1(uint16_t time)  //��ʱ����
{ 
	uint16_t i,j;
	for(i=0;i<time;i++)
  		for(j=0;j<10260;j++);
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** ��������: USART1_IRQHandler
** ��������: �����жϺ���
** ��������: �� 
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void USART1_IRQHandler()
{
	uint8_t TmpReg;

	// ���������жϴ�������
	if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)		
	{
		// �ر�DMA,��ֹ�������������
		DMA_Cmd(DMA1_Channel5, DISABLE);						
		// �ݼ�����
		LenofRxData = DATALEN - DMA_GetCurrDataCounter(DMA1_Channel5);
		DataLen = LenofRxData;

		// ��װ���������ݵĳ���
		DMA_SetCurrDataCounter(DMA1_Channel5, DATALEN);   
		DMA_Cmd(DMA1_Channel5, ENABLE);
		
		 // �ȶ�SR��Ȼ���DR����������ж�״̬
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
