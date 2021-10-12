
#include "usually.h"
#include <stdio.h>

uint8_t  retCRCValue=0x00;


/**
  ***************************************************************
  * @Brief	   : ��������Ƚ��Ƿ����
  * @Parameter : 
  * @Return	   : 1�����  0������
  * @Note	   : 
  ***************************************************************
  */
uint8_t IsEqual(uint8_t *A, uint8_t *B, int Len)
{
	int i=0;
	
	for(i=0;i<Len;i++)
	{
		if(A[i]!=B[i])
		{
			return 0;
		}
	}
	return 1;
}

/**
  ***************************************************************
  * @Brief	   : CRC8_MAXIM У���ʼ��
  * @Parameter : 
  * @Return	   : 
  * @Note	   : 
  ***************************************************************
  */
void CRC8_Init(void)
{
	retCRCValue=0x00;
}

/**
  ***************************************************************
  * @Brief	   : ���ֽڽ���CRC8У��
  * @Parameter : 
  * @Return	   : 
  * @Note	   : 
  ***************************************************************
  */
uint8_t CRC8_MAXIM(uint8_t pData)
{	
	int i=0;
	
	for(i=0;i<8;i++)
	{
		if((retCRCValue^(pData))&0x01)
		{
			retCRCValue^=0x18;
			retCRCValue>>=1;
			retCRCValue|=0x80;
		}
		else
		{
			retCRCValue>>=1;
		}
			pData>>=1;
	}
	return 	retCRCValue;
}


uint8_t DataCompare(const uint8_t *pBuffer1, const uint8_t *pBuffer2, uint16_t LenOfByte)
{
	while(LenOfByte--)
	{
		if(*pBuffer1 != *pBuffer2)
		{
			return 0;
		}
		pBuffer1++;
		pBuffer2++;
	}

	return 1;
}

/**
  ***************************************************************************
  * @Brief	   : ��ָ�����ȵ��ֽ�����ת����������
  * @Parameter : [IN]  pSrcBuf�� ָ���ֽ�����
  *     		 [OUT] pDstBuf�� ָ��������
  *     		 [IN]  LenOfDst��ָ�������ݳ���
  * @Return	   : ��
  ***************************************************************************
  */
void TransB_HW(uint8_t *pSrcBuf,  uint16_t *pDstBuf, uint8_t LenOfDst)
{
	uint8_t i, j;
	
	for (i=0; i<LenOfDst; i++)
	{		
		for (j=0; j<2; j++)
		{
			pDstBuf[i] += (uint32_t)(pSrcBuf[2*i+j]) << (8*(j));		
		}
	}
}


/**
  ***************************************************************************
  * @Brief	   : ��ָ�����ȵ��ֽ�����ת����������
  * @Parameter : [IN]  pSrcBuf�� ָ��������
  *     		 [OUT] pDstBuf�� ָ���ֽ�����
  *     		 [IN]  LenOfSrc��ָ�������ݳ���
  * @Return	   : ��
  ***************************************************************************
  */
void TransHW_B(uint16_t *pSrcBuf, uint8_t *pDstBuf, uint8_t LenOfSrc)
{	
	uint8_t i, j;
		
	for (i=0; i<LenOfSrc; i++)
	{
		for (j=0; j<2; j++)
		{
			pDstBuf[2*i+j] = (pSrcBuf[i] >> (8*(1-j)));		
		}
	}
}


/**
  ****************************************************************************
  * @Brief	   : ��ָ�����ȵ��ֽ����ݽ���������
  * @Parameter : [IN] pDataBuf:  ָ�����������������
  *				 [IN] LenOfByte: ָ���ֽ����ݳ���
  * @Return	   : ���������
  ****************************************************************************
  */ 
uint8_t UHL_CalculateXor(uint8_t *pDataBuf, uint8_t LenOfByte)
{
	uint8_t RetVal;
	
	RetVal = 0;
	if (LenOfByte == 0)
	{
		return RetVal;
	}
	
	do
	{
		RetVal ^= *pDataBuf;
		pDataBuf++;
	}while (--LenOfByte);

	return RetVal;
}


