
#include "spi.h"
#include "usart.h"
#include "usually.h"
#include "string.h"
#include <delay.h>

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: SPIx_Init
** 功能描述: SPI初始化，PB13 PB14 PB15作为SPI2的引脚PC4作为CS
** 参数描述：无
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void SPI2_Init(void)
{
	SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
	
    //SPI2模块对应的PB13:SCK、PB14:MISO、PB15:MOSI为AF引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	// 拉高对应电平
	//GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
	
	//配置PC4即SPI2_NSS
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	//PC4片选信号
	GPIO_SetBits(GPIOC,GPIO_Pin_4);

	//PB12配置从机spi Ready信号管脚，此处与从机PIN13脚相连接	
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
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;			// SPI_NSS_Hard硬件自动，主从1对1，发送数据期间保持NSS为低
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	// the Most Significant Bit
	SPI_InitStructure.SPI_CRCPolynomial = 7;			// 确定CRC计算多项式
	
	SPI_Init(SPI2, &SPI_InitStructure);
//	SPI2->CR1 |= 0x4000;
//	SPI2->CR1 |= 0x2000;

	SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_RXNE);
	SPI_I2S_ClearFlag(SPI2, SPI_I2S_FLAG_TXE);
  	//SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);//开启中断
	
  	SPI_Cmd(SPI2, ENABLE);					   //Enable SPI1  
	
	/* BYS SPI_SR 寄存器 */
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
* Function Name  : SPI_SetSpeed
* Description    :  *APB1(->PCLK1):36M,APB2(->PCLK2):72MPCLK2:SPI1;
* PCLK1:SPI2 SPI3;SPI 时钟最高频率不能高于18MHz
* Input          : uint8_t SpeedSet
*                  如果速度设置输入0，则低速模式，非0则高速模式
*                  SPI_SPEED_HIGH   1
*                  SPI_SPEED_LOW    0
* Output         : None
* Return         : None
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void SPIx_SetSpeed(u8 SpeedSet)
{
    SPI_InitTypeDef SPI_InitStructure ;
    
    SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex ;	// 全双工
    SPI_InitStructure.SPI_Mode=SPI_Mode_Master ;
    SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b ;
    SPI_InitStructure.SPI_CPOL=SPI_CPOL_Low ;
    SPI_InitStructure.SPI_CPHA=SPI_CPHA_1Edge ;
    SPI_InitStructure.SPI_NSS=SPI_NSS_Soft ;		// SPI_NSS_Hard硬件自动，1对1
    if(SpeedSet==SPI_SPEED_LOW)	   					//如果速度设置输入0，则低速模式，非0则高速模式
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
* Description    : SPI读写一个字节（发送完成后返回本次通讯读取的数据）
* Input          : uint8_t TxData 待发送的数
* Output         : None
* Return         : uint8_t RxData 收到的数
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
u8 SPIx_ReadWriteByte(u8 TxData)
{
	// 等待发送空闲
  	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
  	SPI_I2S_SendData(SPI2, TxData);
	
	// 等待接收就绪状态
  	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
  	return SPI_I2S_ReceiveData(SPI2);
}

u8	SPI_ReadByte(void)
{
	// 等待接收就绪状态
  	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

  	return SPI_I2S_ReceiveData(SPI2);
}

//SPI读写函数
u16 SPI_SendByte(u16 byte)
{
   u16 data=0;
   while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)!=SET);	  //等待发送缓冲器为空
   SPI_I2S_SendData(SPI2,byte);
   while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE)==RESET);	  //等待接收缓冲器有数据进来
   data= SPI_I2S_ReceiveData(SPI2);
   return data;
}

void SPI_RXbuff(uint8_t *buff,uint32_t len)
{
	uint32_t i=0;
	
	if(len==0)
		return;
	//delay_ms(1);
	while(!(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)));//等待从机就绪，此处与从机的PIN4脚相连
	GPIO_ResetBits(GPIOC,GPIO_Pin_4);                   //拉低片选信号
	delay_us(10);                                       //必要的10us延时
	for(i=0;i<len;i++)
	{
		buff[i]=SPI_SendByte(0x00);     
		delay_us(10);                                  //必要的10us延时
	}
	GPIO_SetBits(GPIOC,GPIO_Pin_4);                    //拉高片选信号
}

void SPI_TXbuff(uint8_t *buff, uint32_t len)
{
	uint32_t i=0;
	
	if(len==0)
		return;
	
	while(!(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12))); //等待从机就绪，此处与从机的PIN4脚相连
	GPIO_ResetBits(GPIOC,GPIO_Pin_4);                    //拉低片选信号
	delay_us(10);                                        //必要的10us延时
	for(i=0;i<len;i++)
	{
		SPI_SendByte(buff[i]); 
		delay_us(10);                                  //必要的10us延时
	}
	GPIO_SetBits(GPIOC,GPIO_Pin_4);                    //拉高片选信号
}


