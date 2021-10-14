
#include "spi.h"
#include "uart.h"
#include "usually.h"
#include "string.h"
#include <delay.h>
#include "mm32_device.h"
#include "hal_conf.h"
#include "delay.h"

u8 gTxData[256];
u8 gRxData[256];
////////////////////////////////////////////////////////////////////////////////
/// @brief  Read ID
/// @note   None.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void W25xx_ReadID(void)
{
    u8 temp;
    u32 i;
    //Spi cs assign to this pin,select
    W25xx_CS_Low();
    W25xx_ReadWriteByte(RDID);
    for(i = 0; i < 3; i++) {
        temp = W25xx_ReadWriteByte(0x01);
        printf("temp=0x%x\r\n", temp);
    }
    W25xx_CS_High();
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  SPI FLASH Write
/// @note   None.
/// @param  Address:address.
/// @param  number:data length.
/// @param  p:data buff.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void W25xx_PageProgram(u32 address, u8* p, u32 number)
{
    u32 j;
    u8 addr0, addr1, addr2;
    //page address
    address = address & 0xffffff00;
    addr0 = (u8)(address >> 16);
    addr1 = (u8)(address >> 8);
    addr2 = (u8)address;

    W25xx_WriteEnable();
    W25xx_CS_Low();
    W25xx_ReadWriteByte(PP);
    W25xx_ReadWriteByte(addr0);
    W25xx_ReadWriteByte(addr1);
    W25xx_ReadWriteByte(addr2);
    for(j = 0; j < number; j++) {
        W25xx_ReadWriteByte(*(p++));
    }
    W25xx_CS_High();
    W25xx_CheckStatus();
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  SPI Sector Erase
/// @note   None.
/// @param  address:address.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void W25xx_SectorErase(u32 address)
{
    u8 addr0, addr1, addr2;
    address = address & 0xffff0000;
    addr0 = ((u8)(address >> 16)) & 0xff;
    addr1 = ((u8)(address >> 8)) & 0xff;
    addr2 = ((u8)address) & 0xff;

    W25xx_WriteEnable();
    //Spi cs assign to this pin,select
    W25xx_CS_Low();
    W25xx_ReadWriteByte(SE);
    W25xx_ReadWriteByte(addr0);
    W25xx_ReadWriteByte(addr1);
    W25xx_ReadWriteByte(addr2);
    W25xx_CS_High();
    W25xx_CheckStatus();
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  SPI FLASH Read
/// @note   None.
/// @param  Address:address.
/// @param  number:data length.
/// @param  p:data buff.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void W25xx_PageRead(u32 address, u8* p, u32 number)
{
    u8 addr0, addr1, addr2;
    u32 i;
    address = address & 0xffffff00;
    addr0 = (u8)(address >> 16);
    addr1 = (u8)(address >> 8);
    addr2 = (u8)address;

    W25xx_CS_Low();
    W25xx_ReadWriteByte(READ);
    W25xx_ReadWriteByte(addr0);
    W25xx_ReadWriteByte(addr1);
    W25xx_ReadWriteByte(addr2);
    for(i = 0; i < number; i++) {
        gRxData[i] = W25xx_ReadWriteByte(0x00);
    }
    W25xx_CS_High();
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Enable FLASH Read
/// @note   None.
/// @param  None
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void W25xx_WriteEnable(void)
{
    W25xx_CS_Low();
    W25xx_ReadWriteByte(WREN);
    W25xx_CS_High();
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  check Status
/// @note   None.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void W25xx_CheckStatus(void)
{
    u8 temp;
    W25xx_CS_Low();
    W25xx_ReadWriteByte(RDSR);
    while(1) {
        temp = W25xx_ReadWriteByte(0x00);
        if((temp & 0x01) == 0x0)
            break;
    }
    W25xx_CS_High();
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Reset internal NSS pins for selected SPI software
/// @note   None.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void W25xx_CS_Low(void)
{
    GPIO_ResetBits( GPIOB, GPIO_Pin_9 );
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Reset internal NSS pins for selected SPI software
/// @note   None.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void W25xx_CS_High(void)
{
    //Spi cs release
    GPIO_SetBits( GPIOB, GPIO_Pin_9 );
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  SPI FLASH Write Read
/// @note   None.
/// @param  tx_data:data.
/// @retval rx data.
////////////////////////////////////////////////////////////////////////////////
u32 W25xx_ReadWriteByte(u8 tx_data)
{
    SPI_SendData(SPI2, tx_data);
    while (1) {
        if(SPI_GetFlagStatus(SPI2, SPI_FLAG_RXAVL)) {
            return SPI_ReceiveData(SPI2);
        }
    }
}

static void SPI2_GPIO_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_0);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_0);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_0);
	
    GPIO_StructInit(&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
	
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_SetBits(GPIOB,GPIO_Pin_9);
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_13;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_14;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: SPIx_Init
** 功能描述: SPI初始化，PB13 PB14 PB15作为SPI2的引脚PC4作为CS
** 参数描述：无
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void SPI2_Init(void)
{
    SPI_InitTypeDef SPI_InitStruct;
    RCC_APB1PeriphClockCmd(RCC_APB1ENR_SPI2, ENABLE);
	W25xx_CS_Low();

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
	
	SPI_I2S_ClearFlag(SPI2, SPI_IT_RX | SPI_IT_TXEPT);
    SPI_BiDirectionalLineConfig(SPI2, SPI_Direction_Rx);
    SPI_BiDirectionalLineConfig(SPI2, SPI_Direction_Tx);
    SPI_Cmd(SPI2, ENABLE);
	
	SPI2_GPIO_Config();
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
    
//    SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex ;	// 全双工
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
//    SPI_InitStructure.SPI_CRCPolynomial=7 ;
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
  	while (SPI_GetFlagStatus(SPI2, SPI_FLAG_TXEPT) == RESET);
  	SPI_SendData(SPI2, TxData);
	
	// 等待接收就绪状态
  	while (SPI_GetFlagStatus(SPI2, SPI_FLAG_RXAVL) == RESET);
  	return SPI_ReceiveData(SPI2);
}

u8	SPI_ReadByte(void)
{
	// 等待接收就绪状态
  	while (SPI_GetFlagStatus(SPI2, SPI_FLAG_RXAVL) == RESET);

  	return SPI_ReceiveData(SPI2);
}

//SPI读写函数
u16 SPI_SendByte(u16 byte)
{
   u16 data=0;
   while(SPI_GetFlagStatus(SPI2,SPI_FLAG_TXEPT)!=SET);	  //等待发送缓冲器为空
   SPI_SendData(SPI2,byte);
   while(SPI_GetFlagStatus(SPI2,SPI_FLAG_RXAVL)==RESET);	  //等待接收缓冲器有数据进来
   data= SPI_ReceiveData(SPI2);
   return data;
}

void SPI_RXbuff(uint8_t *buff,uint32_t len)
{
	uint32_t i=0;
	
	if(len==0)
		return;
	//delay_ms(1);
	while(!(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)));//等待从机就绪，此处与从机的PIN4脚相连
	GPIO_ResetBits(GPIOB,GPIO_Pin_9);                   //拉低片选信号
	DELAY_Us(10);                                       //必要的10us延时
	for(i=0;i<len;i++)
	{
		buff[i]=SPI_SendByte(0x00);     
		DELAY_Us(10);                                  //必要的10us延时
	}
	GPIO_SetBits(GPIOB,GPIO_Pin_9);                    //拉高片选信号
//	W25xx_PageRead(0, buff, len);
////	W25xx_PageProgram(0, buff, len);
}

void SPI_TXbuff(uint8_t *buff, uint32_t len)
{
	uint32_t i=0;
	
	if(len==0)
		return;
	
	while(!(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12))); //等待从机就绪，此处与从机的PIN4脚相连
	GPIO_ResetBits(GPIOB,GPIO_Pin_9);                    //拉低片选信号
	W25xx_CS_Low();
	//DELAY_Ms(20);                                        //必要的10us延时
	for(i=0;i<len;i++)
	{
		SPI_SendByte(buff[i]); 
		DELAY_Us(20);                                  //必要的10us延时
	}
	GPIO_SetBits(GPIOB,GPIO_Pin_9);                    //拉高片选信号
	
//	W25xx_PageProgram(0, buff, len);
////	W25xx_PageRead(0, buff, len);
}


