/** * * * * * * * * * * * * * *
 * @file   : mapping.h
 * @time   : 10/18/21 10:24:27
 * @author : Yuule
 * */

#ifndef _MAPPING_H
#define _MAPPING_H

#include "stdint.h"
#include "string.h"
#include "stdio.h"

typedef struct 
{
    char *cmd;
    uint8_t content_cmd[6];
    uint8_t key_cmd[6];
}mapping_t;

extern int splitflag;
extern int cmd_index;
extern mapping_t cmd_table[];
extern char Cmd_Data1[256];
extern char Cmd_Data2[256];
extern uint8_t Cmd_DataLen1;
extern uint8_t Cmd_DataLen2;

int string_split(char *strp, uint32_t strsize, char ch, char *argv[], uint32_t argcM );
int at_cmd_parse(uint8_t *pdata, uint16_t size);

#endif
