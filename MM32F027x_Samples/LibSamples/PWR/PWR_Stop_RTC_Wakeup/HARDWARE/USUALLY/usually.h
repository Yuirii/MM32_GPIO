#ifndef __USUALLY_H
#define __USUALLY_H

//头文件调用
#include "reg_common.h"
//#include "hardware.h"

////LED引脚定义
//#define LED1 PGout(14)	   	//板子上对应于LD1

//#define LED2 PDout(13)	   	//板子上对应于LD1


extern void CRC8_Init(void);
extern uint8_t CRC8_MAXIM(uint8_t pData);
extern uint8_t IsEqual(uint8_t *A, uint8_t *B, int Len);

extern uint8_t UHL_CalculateXor(uint8_t *pDataBuf, uint8_t LenOfByte);
extern void TransHW_B(uint16_t *pSrcBuf, uint8_t *pDstBuf, uint8_t LenOfSrc);
extern void TransB_HW(uint8_t *pSrcBuf,  uint16_t *pDstBuf, uint8_t LenOfDst);
extern uint8_t DataCompare(const uint8_t *pBuffer1, const uint8_t *pBuffer2, uint16_t LenOfByte);

#endif
