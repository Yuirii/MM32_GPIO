#ifndef __VERIFYPRO_H
#define __VERIFYPRO_H			    

#include "stm32f10x.h"
#include "hardware.h"

#define	SPI_401S_CS PGout(1)  //ѡ��401SоƬ					 

extern uint8_t SPI_CRC32_Check;

/************************�������ú���************************/
extern void Init_APP(void);
extern void CRC32_MPEG_2(const uint8_t *data, int length, unsigned char crc_Buf[]);
extern uint16_t STM_OperationHandle(uint8_t *cmd, uint8_t *data, uint16_t d_len, uint8_t *result, uint16_t *r_len);


#endif

