/** * * * * * * * * * * * * * *
 * @file   : mapping.c
 * @time   : 10/18/21 10:24:27
 * @author : Yuule
 * */
#include "mapping.h"

#define respond_error()	printf("ERROR\r\n")
#define respond_ok()	printf("OK\r\n");
#define ARGC_LIMIT      2

int splitflag = 0;
int cmd_index = NULL;
char Cmd_Data1[256];
char Cmd_Data2[256];
uint8_t Cmd_DataLen1 = 0;
uint8_t Cmd_DataLen2 = 0;
mapping_t cmd_table[] = {
    {"SM3", 
    {0xAA, 0xC6, 0x00, 0xA1, 0x00, 0x00}, 
    {0x00}},

    {"SM4", //AAC300A10010, AAC300A20010
    {0xAA, 0xC3, 0x00, 0xA1, 0x00, 0x00}, 
    {0xAA, 0xC3, 0x00, 0xA2, 0x00, 0x00}},

    {"AES", //AAC000A1000a, AAC000A20020
    {0xAA, 0xC0, 0x00, 0xA1, 0x00, 0x00}, 
    {0xAA, 0xC0, 0x00, 0xA2, 0x00, 0x00}},

    {"DES", //AAC100A10010, AAC100A20008
    {0xAA, 0xC1, 0x00, 0xA1, 0x00, 0x00}, 
    {0xAA, 0xC1, 0x00, 0xA2, 0x00, 0x00}},

    {"SM2", //AAC500A10010, AAC500A20040
    {0xAA, 0xC5, 0x00, 0xA1, 0x00, 0x00}, 
    {0xAA, 0xC5, 0x00, 0xA2, 0x00, 0x00}},
    
    {"RSA", //AAC400A50010 , AAC410A20100  
    {0xAA, 0xC4, 0x00, 0xA5, 0x00, 0x00}, 
    {0xAA, 0xC4, 0x10, 0xA2, 0x00, 0x00}},
};

#define CMD_TABLE_SIZE  (sizeof(cmd_table) / sizeof(mapping_t))


/**@brief 	    string split.
 * @param[in] 	ch: split on a 'ch' character.
 * @param[in] 	
 * @return 	    split into number.
 **/
int string_split(char *strp, uint32_t strsize, char ch, char *argv[], uint32_t argcM )
{
	int ch_index = 0;
	int argc_index = 0;
	uint8_t spliflag = 0;
	
	if ((!strsize) || (!argcM)) return 0;

	argv[argc_index++] = &strp[ch_index];
	for (ch_index = 0; ch_index < strsize; ch_index++) {
		if (strp[ch_index] == ch) {
			strp[ch_index] = '\0';
#if 0	/* 处理连续出现ch情况 */
			if (1 == splitflag) {
				argv[argc_index++] = &strp[ch_index];
			}
#endif			
			splitflag = 1;
			Cmd_DataLen1 = ch_index;
		} else if (splitflag == 1) {
			splitflag = 0;
			argv[argc_index++] = &strp[ch_index];
			if (argc_index >= argcM) break;
		} else {
			splitflag = 0;
		}
	}
	
	return argc_index;
}

/**@brief 	    parse cmd.
 * @param[in] 	
 * @param[in] 	
 * @return 	    NUM OF TABLE.
 **/
int at_cmd_parse(uint8_t *pdata, uint16_t size)
{
	int ret  = -1;
	char *ptr = NULL;
	int argc = ARGC_LIMIT;
	uint16_t offset = 0;
	int index = 0;
	char *argv[ARGC_LIMIT] = { (char *)0 };

	for (index = 0; index < CMD_TABLE_SIZE; index++) {
		ptr = strstr((const char *)pdata, cmd_table[index].cmd);
		if (ptr != NULL) {
			ptr += strlen(cmd_table[index].cmd);
			offset = ptr - (char *)pdata;
            cmd_index = index;
			break;
		}
	}
	if (index >= CMD_TABLE_SIZE) goto at_end;

	/* 解析查询命令 */
    if (ptr[0] == ',') { /* 解析设置命令 */
		ptr += 1;
		argc = string_split((char*)ptr, size - offset, ',', argv, argc);
		Cmd_DataLen2 = size - offset - Cmd_DataLen1 - 2 - 2;
		memcpy(Cmd_Data1, &argv[0][0], Cmd_DataLen1);
		memcpy(Cmd_Data2, &argv[1][0], Cmd_DataLen2);
		ret = 0;
	} else {
		ret = -1;
	}

at_end:
	if (-1 == ret) respond_error();
	else respond_ok();

	return ret;
}



