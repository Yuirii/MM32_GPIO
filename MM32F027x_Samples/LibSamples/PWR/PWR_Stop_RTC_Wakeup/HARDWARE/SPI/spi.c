
#include "spi.h"
#include "usart.h"
#include "usually.h"
#include "string.h"
#include <delay.h>

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** ��������: SPIx_Init
** ��������: SPI��ʼ����PB13 PB14 PB15��ΪSPI2������PC4��ΪCS
** ������������
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void SPI2_Init(void)
{
	SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
	
    //SPI2ģ���Ӧ��PB13:SCK��PB14:MISO��PB15:MOSIΪAF����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	// ���߶�Ӧ��ƽ
	//GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
	
	//����PC4��SPI2_NSS
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	//PC4Ƭѡ�ź�
	GPIO_SetBits(GPIOC,GPIO_Pin_4);

	//PB12���ôӻ�spi Ready�źŹܽţ��˴���ӻ�PIN13��������	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	
	/* SPI2 configuration pclk1 2*36M */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;			// SPI_NSS_HardӲ���Զ�������1��1�����������ڼ䱣��NSSΪ��
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	// the Most Significant Bit
	SPI_InitStructure.SPI_CRCPolynomial = 7;			// ȷ��CRC�������ʽ
	
	SPI_Init(SPI2, &SPI_InitStructure);
//	SPI2->CR1 |= 0x4000;
//	SPI2->CR1 |= 0x2000;

	SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_RXNE);
	SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_TXE);
  	//SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);//�����ж�
	
  	SPI_Cmd(SPI2, ENABLE);					   //Enable SPI1  
	
	/* BYS SPI_SR �Ĵ��� */
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
* Function Name  : SPI_SetSpeed
* Description    :  *APB1(->PCLK1):36M,APB2(->PCLK2):72MPCLK2:SPI1;
* PCLK1:SPI2 SPI3;SPI ʱ�����Ƶ�ʲ��ܸ���18MHz
* Input          : uint8_t SpeedSet
*                  ����ٶ���������0�������ģʽ����0�����ģʽ
*                  SPI_SPEED_HIGH   1
*                  SPI_SPEED_LOW    0
* Output         : None
* Return         : None
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void SPIx_SetSpeed(u8 SpeedSet)
{
    SPI_InitTypeDef SPI_InitStructure ;
    
    SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex ;	// ȫ˫��
    SPI_InitStructure.SPI_Mode=SPI_Mode_Master ;
    SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b ;
    SPI_InitStructure.SPI_CPOL=SPI_CPOL_Low ;
    SPI_InitStructure.SPI_CPHA=SPI_CPHA_1Edge ;
    SPI_InitStructure.SPI_NSS=SPI_NSS_Soft ;		// SPI_NSS_HardӲ���Զ���1��1
    if(SpeedSet==SPI_SPEED_LOW)	   					//����ٶ���������0�������ģʽ����0�����ģʽ
    {
        SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_256;
    }
    else 
    {
        SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_4;
    }

    SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB ;
    SPI_InitStructure.SPI_CRCPolynomial=7 ;
    SPI_Init(SPI2,&SPI_InitStructure);
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
* Function Name  : SPI_ReadWriteByte
* Description    : SPI��дһ���ֽڣ�������ɺ󷵻ر���ͨѶ��ȡ�����ݣ�
* Input          : uint8_t TxData �����͵���
* Output         : None
* Return         : uint8_t RxData �յ�����
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
u8 SPIx_ReadWriteByte(u8 TxData)
{
	// �ȴ����Ϳ���
  	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
  	SPI_I2S_SendData(SPI2, TxData);
	
	// �ȴ����վ���״̬
  	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
  	return SPI_I2S_ReceiveData(SPI2);
}

u8	SPI_ReadByte(void)
{
	// �ȴ����վ���״̬
  	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

  	return SPI_I2S_ReceiveData(SPI2);
}

//SPI��д����
u16 SPI_SendByte(u16 byte)
{
   u16 data=0;
   while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)!=SET);	  //�ȴ����ͻ�����Ϊ��
   SPI_I2S_SendData(SPI2,byte);
   while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE)==RESET);	  //�ȴ����ջ����������ݽ���
   data= SPI_I2S_ReceiveData(SPI2);
   return data;
}

void SPI_RXbuff(uint8_t *buff,uint32_t len)
{
	uint32_t i=0;
	
	if(len==0)
		return;
	//delay_ms(1);
	while(!(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)));//�ȴ��ӻ��������˴���ӻ���PIN4������
	GPIO_ResetBits(GPIOC,GPIO_Pin_4);                   //����Ƭѡ�ź�
	delay_us(10);                                       //��Ҫ��10us��ʱ
	for(i=0;i<len;i++)
	{
		buff[i]=SPI_SendByte(0x00);     
		delay_us(10);                                  //��Ҫ��10us��ʱ
	}
	GPIO_SetBits(GPIOC,GPIO_Pin_4);                    //����Ƭѡ�ź�
}

void SPI_TXbuff(uint8_t *buff, uint32_t len)
{
	uint32_t i=0;
	
	if(len==0)
		return;
	
	while(!(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12))); //�ȴ��ӻ��������˴���ӻ���PIN4������
	GPIO_ResetBits(GPIOC,GPIO_Pin_4);                    //����Ƭѡ�ź�
	delay_us(10);                                        //��Ҫ��10us��ʱ
	for(i=0;i<len;i++)
	{
		SPI_SendByte(buff[i]); 
		delay_us(10);                                  //��Ҫ��10us��ʱ
	}
	GPIO_SetBits(GPIOC,GPIO_Pin_4);                    //����Ƭѡ�ź�
}


