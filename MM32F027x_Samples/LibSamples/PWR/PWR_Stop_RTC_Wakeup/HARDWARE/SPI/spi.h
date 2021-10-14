#ifndef __SPI_H
#define __SPI_H
#include "usually.h"

//#include "sys.h"
//SPI 驱动 V1.1
//2010/5/13	
/* SPI总线速度设置*/
#define SPI_SPEED_LOW   0
#define SPI_SPEED_HIGH  1

// SPI总线速度设置 
#define SPI_SPEED_2   0
#define SPI_SPEED_4   1
#define SPI_SPEED_8   2
#define SPI_SPEED_16  3
#define SPI_SPEED_256 4

// copy form MM32 SPI.H
#define SPI_MOSI_BUSCLK                  RCC_AHBENR_GPIOB
#define SPI_MOSI_PIN                     GPIO_Pin_15
#define SPI_MOSI_PORT                    GPIOB
#define SPI_MOSI_AFSOURCE                GPIO_PinSource15
#define SPI_MOSI_AFMODE                  GPIO_AF_0

#define SPI_NSS_BUSCLK                  RCC_AHBENR_GPIOC
#define SPI_NSS_PIN                     GPIO_Pin_4
#define SPI_NSS_PORT                    GPIOC
#define SPI_NSS_AFSOURCE                GPIO_PinSource4
#define SPI_NSS_AFMODE                  GPIO_AF_0

#define SPI_MISO_BUSCLK                  RCC_AHBENR_GPIOB
#define SPI_MISO_PIN                     GPIO_Pin_14
#define SPI_MISO_PORT                    GPIOB
#define SPI_MISO_AFSOURCE                GPIO_PinSource14
#define SPI_MISO_AFMODE                  GPIO_AF_0

#define SPI_SCK_BUSCLK                  RCC_AHBENR_GPIOB
#define SPI_SCK_PIN                     GPIO_Pin_13
#define SPI_SCK_PORT                    GPIOB
#define SPI_SCK_AFSOURCE                GPIO_PinSource13
#define SPI_SCK_AFMODE                  GPIO_AF_0
			

#define READ                  0x03
#define FAST_READ             0x0B
#define RDID                  0x9F
#define WREN                  0x06
#define WRDI                  0x04
#define SE                    0xD8
#define BE                    0xC7
#define PP                    0x02
#define RDSR                  0x05
#define WRSR                  0x01
#define DP                    0xB9
#define RES                   0xAB


void SPI2_Init(void);			 //初始化SPI口
void SPIx_SetSpeed(u8 SpeedSet); //设置SPI速度   
//u8 SPIx_ReadWriteByte(u8 TxData);//SPI总线读写一个字节

//u8	SPI_ReadByte(void);
//u8 SPIx_ReadWriteByte(u8 TxData);

uint32_t CRC32(uint8_t *pBuf, uint16_t nSize);

extern void SPI_Configuration(void);
extern u16 SPI_SendByte(u16 byte);
extern void SPI_RXbuff(uint8_t *buff,uint32_t len);
extern void SPI_TXbuff(uint8_t *buff, uint32_t len);


void W25xx_ReadID(void);
void W25xx_SectorErase(u32 address);
void W25xx_PageProgram(u32 address, u8* p, u32 number);
void W25xx_PageRead(u32 address, u8* p, u32 number);
void W25xx_CheckStatus(void);
void W25xx_WriteEnable(void);
void W25xx_CS_High(void);
void W25xx_CS_Low(void);
u32 W25xx_ReadWriteByte(u8 tx_data);
#endif

