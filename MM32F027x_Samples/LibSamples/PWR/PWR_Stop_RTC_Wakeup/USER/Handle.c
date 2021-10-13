/****************************************************************************
*			    Copyright 2018 by WuHan Runjetic, All rights reserved.
*****************************************************************************
*@Filename :  Handle.c
*@Author   :  Yuwei
*@Date     :  2018-12-26
*@Brief    :  数据处理文件
*****************************************************************************
*
*/

#include "Handle.h"
#include "uart.h"
#include "delay.h"
#include "usually.h"
#include "verifypro.h"

uint8_t cmd[6] = {0};
uint16_t send_len = 0;
uint16_t result_len = 0;
uint8_t send_data[DATALEN] = {0};
uint8_t result_data[DATALEN] = {0};

uint16_t Cmd_DataLen = 0;
uint8_t Cmd_APDU[6] = {0};
uint8_t Cmd_Data[DATALEN] = {0};


uint16_t SendData_API(void)
{
	uint16_t ResultSW = 0X00;		// 最终结果返回状态
	uint8_t ResultByte[2] = {0};	// 发送命令后返回状态
	uint16_t data_len = 0, i;
	uint16_t t_time = 0;
	
	for(i = 0; i < 6; i++)
	{
		cmd[i] = Cmd_APDU[i];
	}
	data_len=(uint16_t)cmd[4]<<8 | (uint16_t)cmd[5];
	
	t_time = 500;
	while(1){
		DELAY_Ms(2);
		if(data_len == Cmd_DataLen || Cmd_DataLen == 0) {
			break;
		}
		if(DataLen){
			memcpy(&Cmd_Data[Cmd_DataLen], DATA, DataLen);
			Cmd_DataLen += DataLen;
			DataLen = 0;
		}
		if(t_time-- <= 0)
			break;
	}
	
	if(data_len != Cmd_DataLen && Cmd_DataLen > 0) {
		RJPrintInfo("\r\n 数据长度或格式错误 ", 0, 0);
		return 0x5E5E;
	}
	memset(result_data, 0, sizeof(result_data));
	result_len = 0;
	send_len = Cmd_DataLen;
	
	for(i = 0; i < send_len; i++)
	{
		send_data[i] = Cmd_Data[i];
	}
	
	RJPrintInfo("\r\n Send Cmd: ", cmd, 6);
	RJPrintInfo("\r\n Send Data: ", send_data, send_len);
	ResultSW = STM_OperationHandle(cmd, send_data, send_len, result_data, &result_len);
	RJPrintInfo("\r\n Data Received: ", result_data, result_len);
	
	Cmd_DataLen = 0;
	memset(Cmd_APDU, 0, sizeof(Cmd_APDU));
	memset(Cmd_Data, 0, sizeof(Cmd_Data));
	TransHW_B(&ResultSW , ResultByte, 1);
	RJPrintInfo("\r\n ResultSW: ", ResultByte, 2);
	RJPrintInfo("\r\n Finish Task! ",0,0);
	
	return ResultSW;
}

void CommomOperation(void)
{
	if(DataLen>6){
		Cmd_DataLen = DataLen-6;
		memcpy(Cmd_APDU, DATA, 6);
		memcpy(Cmd_Data, &DATA[6], Cmd_DataLen);
	}else if(DataLen==6){
		Cmd_DataLen = 0;
		memcpy(Cmd_APDU, DATA, 6);
	}else{
		RJPrintInfo("\r\n 数据错误", 0, 0);
		return;
	}
	
	DataLen = 0;
	SendData_API();
}

/***********************END OF FILE****************************************************************/
