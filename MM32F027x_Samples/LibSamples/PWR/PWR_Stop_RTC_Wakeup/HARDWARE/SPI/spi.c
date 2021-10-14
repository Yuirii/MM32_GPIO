
#include "spi.h"
#include "uart.h"
#include "usually.h"
#include "string.h"
#include <delay.h>
#include "mm32_device.h"
#include "hal_conf.h"


static void W25xx_CS_High()
{
    //Spi cs release
    SPI_CSInternalSelected(SPI2, DISABLE);
}

static void SPI2_GPIO_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOB, ENABLE);
    GPIO_PinAFConfig(SPI_MOSI_PORT, SPI_MOSI_AFSOURCE, SPI_MOSI_AFMODE);
    GPIO_PinAFConfig(SPI_NSS_PORT, SPI_NSS_AFSOURCE, SPI_NSS_AFMODE);
    GPIO_PinAFConfig(SPI_MISO_PORT, SPI_MISO_AFSOURCE, SPI_MISO_AFMODE);
    GPIO_PinAFConfig(SPI_SCK_PORT, SPI_SCK_AFSOURCE, SPI_SCK_AFMODE);
    W25xx_CS_High();
    //spi2_cs  pb12
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin  = SPI_NSS_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(SPI_NSS_PORT, &GPIO_InitStruct);
    //spi2_sck  pb13
    GPIO_InitStruct.GPIO_Pin  = SPI_SCK_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(SPI_SCK_PORT, &GPIO_InitStruct);
    //spi2_mosi  pb15
    GPIO_InitStruct.GPIO_Pin  = SPI_MOSI_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(SPI_MOSI_PORT, &GPIO_InitStruct);
    //spi2_miso  pb14
    GPIO_InitStruct.GPIO_Pin  = SPI_MISO_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(SPI_MISO_PORT, &GPIO_InitStruct);

}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** ��������: SPIx_Init
** ��������: SPI��ʼ����PB13 PB14 PB15��ΪSPI2������PC4��ΪCS
** ������������
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void SPI2_Init(void)
{
    SPI_InitTypeDef SPI_InitStruct;
    RCC_APB1PeriphClockCmd(RCC_APB1ENR_SPI2, ENABLE);

	SPI2_GPIO_Config();
    SPI_StructInit(&SPI_InitStruct);
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_DataWidth = SPI_DataWidth_8b;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStruct.SPI_BaudRatePrescaler = (SPI_BaudRatePrescaler_TypeDef)SPI_BaudRatePrescaler_32;
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_Init(SPI2, &SPI_InitStruct);
    if(SPI_InitStruct.SPI_BaudRatePrescaler <= 8) {
        exSPI_DataEdgeAdjust(SPI2, SPI_DataEdgeAdjust_FAST);
    }
    SPI_BiDirectionalLineConfig(SPI2, SPI_Direction_Rx);
    SPI_BiDirectionalLineConfig(SPI2, SPI_Direction_Tx);
    SPI_Cmd(SPI2, ENABLE);
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
    
//    SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex ;	// ȫ˫��
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
//    SPI_InitStructure.SPI_CRCPolynomial=7 ;
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
  	while (SPI_GetFlagStatus(SPI2, SPI_FLAG_TXEPT) == RESET);
  	SPI_SendData(SPI2, TxData);
	
	// �ȴ����վ���״̬
  	while (SPI_GetFlagStatus(SPI2, SPI_FLAG_RXAVL) == RESET);
  	return SPI_ReceiveData(SPI2);
}

u8	SPI_ReadByte(void)
{
	// �ȴ����վ���״̬
  	while (SPI_GetFlagStatus(SPI2, SPI_FLAG_RXAVL) == RESET);

  	return SPI_ReceiveData(SPI2);
}

//SPI��д����
u16 SPI_SendByte(u16 byte)
{
   u16 data=0;
   while(SPI_GetFlagStatus(SPI2,SPI_FLAG_TXEPT)!=SET);	  //�ȴ����ͻ�����Ϊ��
   SPI_SendData(SPI2,byte);
   while(SPI_GetFlagStatus(SPI2,SPI_FLAG_RXAVL)==RESET);	  //�ȴ����ջ����������ݽ���
   data= SPI_ReceiveData(SPI2);
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
	DELAY_Ms(1);                                       //��Ҫ��10us��ʱ
	for(i=0;i<len;i++)
	{
		buff[i]=SPI_SendByte(0x00);     
		DELAY_Ms(1);                                  //��Ҫ��10us��ʱ
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
	DELAY_Ms(1);                                        //��Ҫ��10us��ʱ
	for(i=0;i<len;i++)
	{
		SPI_SendByte(buff[i]); 
		DELAY_Ms(1);                                  //��Ҫ��10us��ʱ
	}
	GPIO_SetBits(GPIOC,GPIO_Pin_4);                    //����Ƭѡ�ź�
}


