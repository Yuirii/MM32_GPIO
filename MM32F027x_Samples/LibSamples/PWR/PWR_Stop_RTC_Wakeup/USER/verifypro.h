#ifndef __VERIFYPRO_H
#define __VERIFYPRO_H			    

#include "mm32_device.h"
#include "hal_conf.h"
//#include "hardware.h"

#define	SPI_401S_CS PGout(1)  //选中401S芯片
// RJGT401S复位控制引脚宏定义
#define GPIO_Port_Reset             GPIOB
#define GPIO_Pin_Reset        		GPIO_Pin_11
#define RJGT401S_RESET							if(1)\
	do											\
	{											\
		GPIO_Port_Reset->BSRR = GPIO_Pin_Reset;	\
		DELAY_Ms(1);							\
		GPIO_Port_Reset->BRR = GPIO_Pin_Reset;	\
		DELAY_Ms(1);							\
	} while(0)


extern uint8_t SPI_CRC32_Check;

/************************基本调用函数************************/
extern void Init_APP(void);
extern void CRC32_MPEG_2(const uint8_t *data, int length, unsigned char crc_Buf[]);
extern uint16_t STM_OperationHandle(uint8_t *cmd, uint8_t *data, uint16_t d_len, uint8_t *result, uint16_t *r_len);


#endif

