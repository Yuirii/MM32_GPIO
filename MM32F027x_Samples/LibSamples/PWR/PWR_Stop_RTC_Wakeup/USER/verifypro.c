
#include "verifypro.h"
#include "platform.h"
#include "string.h"
#include "usart.h"
#include "delay.h"
#include "spi.h"

uint8_t SPI_CRC32_Check = 1;

uint8_t send_buff[DATALEN] = {0};
uint8_t recv_buff[DATALEN] = {0};


#define Get_SlaveStatus()  GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)

void Reset_401S(void)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_11);	         //P1.28����ߵ�ƽ
	delay_ms(2);
	GPIO_ResetBits(GPIOB,GPIO_Pin_11);        //P1.28����͵�ƽ
}

void Init_APP(void)
{
	RXflag = 0;
	DataLen = 0x26;
	SPI2_Init();		   									//��ʼ��SPI	 
}

uint8_t WaitNotBusy(void)
{
	int tree=0;
	while(!(Get_SlaveStatus()))
	{
		if(tree > 500)
		{
			printf("time out !\r\n");
			return 1;
		}
		tree++;
		delay_ms(15);
	}
	
	return 0;
}

void CRC32_MPEG_2(const uint8_t *data, int length, unsigned char crc_Buf[])
{
	unsigned char i;
	int CRC32 = 0xFFFFFFFF, j = 0;
	while ((length--) != 0)
	{
		CRC32 ^= (uint32_t)data[j] << 24;
		j++;
		for (i = 0; i < 8; ++i)
		{
			if ((CRC32 & 0x80000000) != 0)
				CRC32 = (CRC32 << 1) ^ 0x04C11DB7;
			else
				CRC32 <<= 1;
		}
	}
	
	//Reverse and wordtobyte
	for(i=0; i<4; i++)
	{
		crc_Buf[3-i] = (CRC32>>(8*i))&0xFF;		
	}
}

uint8_t Check_CRC32(const uint8_t *data, uint16_t len, const uint8_t *CRC32)
{
	uint8_t r_Buf[4] = {0};

	CRC32_MPEG_2(data, len, r_Buf);
	if(DataCompare(r_Buf, CRC32, 4))
	{
		return 1;
	} else
		return 0;
}

uint8_t calculateXor(const uint8_t *data, uint16_t len)
{
	uint8_t xor_byte = 0;
	uint16_t i;
	for(i = 0; i < len; i++)
		xor_byte ^= data[i];
	return xor_byte;
}

uint8_t Check_Xor(const uint8_t *data, uint16_t len, const uint8_t *CRC8)
{
	uint8_t Xor_Result = 0;
	Xor_Result = calculateXor(data, len);
	if(DataCompare(&Xor_Result, CRC8, 1))
	{
		return 1;
	} else
		return 0;
}

uint8_t CheckCRC(const uint8_t *data, uint16_t len)
{
	if(SPI_CRC32_Check)
	{
		return Check_CRC32(data, len-4, &data[len-4]);
	}else{
		return Check_Xor(data, len-1, &data[len-1]);
	}
}

/*********************************************************
 * �������̣�
 * 1.�ȷ���6�ֽ�����
 * 2.����״̬�֣�0x9000ָ��������0x6E00ָ���������������״̬�ֽڱ�ʾ�д���������
 *********************************************************/
static uint16_t STM_CommomOpera(uint8_t *buff, uint16_t len, uint8_t *result, uint16_t *rlen)
{
	uint8_t CMD_Result[2] = {0x00}, Result_SW[2] = {0X00};
	uint16_t Result_status = 0;
	
	*rlen = 0;
	SPI_TXbuff(buff, 0x06);      // ��������
	delay_us(50);
	SPI_RXbuff(CMD_Result, 2);    // ��CMD����״̬�ֽ�
	RJPrintInfo("\r\n Receive CMD_Result : ", CMD_Result, 2);
	if((CMD_Result[0] == 0x9A) && (CMD_Result[1] == 0x00))   // CMD��������
    {
	   if(len == 0x06 && (buff[4] != 0 || buff[5] != 0))  //�����ݣ����д���������
	   {
			delay_us(50);
			// �����������1.�д����յ����ݣ�2.�޴����յ�����
			Result_status = (uint16_t)buff[4]<<8 | (uint16_t)buff[5];
			SPI_RXbuff(result,Result_status);		//���ս������
			
			if(CheckCRC(result, Result_status)){
				*rlen = Result_status - (SPI_CRC32_Check*3+1);
				return  *rlen;
			}
			return Result_status;
	   }
	   else if(len > 0x06)   //������
	   {
		   delay_us(50);
		   SPI_TXbuff(&buff[6], len-0x06);  //��������
	   }
	}
	else
	{
		Result_status=(uint16_t)CMD_Result[0]<<8 | (uint16_t)CMD_Result[1] ;
		return Result_status;
	}
	
	delay_us(100);
	SPI_RXbuff(Result_SW,2);    			//�����״̬�ֽ�
	RJPrintInfo("\r\n Received Result_SW : ", Result_SW, 2);
	
	Result_status=(uint16_t)Result_SW[0]<<8 | (uint16_t)Result_SW[1] ;
	if(Result_status==0x9000||Result_status==0x6A00||Result_status==0x6B00||Result_status==0x6C00 
	  ||Result_status==0x6D00||Result_status==0x6E00||Result_status==0x6F00)   //��������ȡ��
	{
		return  Result_status;
	}
	
	while(Result_status > 1024+256){
		//����Ӹ�λ
		RJGT401S_RESET;
		return Result_status;
	}
	SPI_RXbuff(result, Result_status);
	
	if(Result_status <= (SPI_CRC32_Check*3+1)) return 0x00;
	if(CheckCRC(result, Result_status)){
		*rlen = Result_status - (SPI_CRC32_Check*3+1);
		return  *rlen;
	}
	return Result_status;
}

/**
  ***************************************************************************
  * @Brief	   : 	1.����ָ��;2.����������ݣ�3.���շ�������
  * @Parameter : 	[IN ] cmd		������
  * 				[IN ] data		�����͵����ݣ�
  * 				[IN ] len		�������ݳ��ȣ�
  * 				[OUT] result	���ص����ݣ�
  * 				[OUT] r_len		�������ݳ���
  * @Return	   : 	����״̬
  * @Caller	   : 	���������
  ***************************************************************************
  */
uint16_t STM_OperationHandle(uint8_t *cmd, uint8_t *data, uint16_t d_len, uint8_t *result, uint16_t *r_len)
{
	int i = 0;
	uint8_t send_xor;
	uint8_t resultbuff[4] = {0};
	uint16_t send_len = d_len + 6;
	uint16_t Result_SW = 0x00;
	
	if(WaitNotBusy()) return 0;
	
	for(i = 0; i < 6; i++)
	{
		send_buff[i] = cmd[i];
	}
	
	if(d_len > 0)
	{
		send_xor = 0;
		for(i = 6; i < send_len; i++)
		{
			send_buff[i] = data[i-6];
			send_xor ^= send_buff[i];
		}
		if(SPI_CRC32_Check) {
			CRC32_MPEG_2(data, d_len, resultbuff);
			send_buff[send_len++] = resultbuff[0];
			send_buff[send_len++] = resultbuff[1];
			send_buff[send_len++] = resultbuff[2];
			send_buff[send_len++] = resultbuff[3];
			
			d_len += 4;		// ����У���
			send_buff[4] = (d_len & 0xFF00)>>8;
			send_buff[5] = (d_len & 0xFF);
		}else {
			send_buff[send_len++] = send_xor;
			
			d_len++;		// ����У���
			send_buff[4] = (d_len & 0xFF00)>>8;
			send_buff[5] = (d_len & 0xFF);
		}
	}
	
	Result_SW = STM_CommomOpera(send_buff, send_len, recv_buff, r_len);
	if(Result_SW==0x9000||Result_SW==0x6A00||Result_SW==0x6B00||Result_SW==0x6C00 
			  ||Result_SW==0x6D00||Result_SW==0x6E00||Result_SW==0x6F00){
		return Result_SW;
	}else if(*r_len > 0) {
		Result_SW = *r_len;
		memcpy(result, recv_buff, *r_len);
	}
	delay_us(10);
	
	return Result_SW;
}



