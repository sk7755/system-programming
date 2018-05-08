#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define ERROR_HEX_RANGE -1000
#define ERROR_HEX_FORMAT -1001
#define ERROR_MEM_BOUND -1002
#define ERROR_OVER_ARG -1003
#define ERROR_LACK_ARG -1004
#define ERROR_SE_RANGE -1005
#define ERROR_VALUE_RANGE -1006
#define ERROR_NOT_FOUND -1007
#define ERROR_FILE_OPEN -1008
#define ERROR_FILE_CLOSE -1009
#define ERROR_NOT_MNEMONIC -1010
#define ERROR_DIR_OPEN -1011
#define ERROR_DIR_CLOSE -1012
#define ERROR_EMPTY_ASM -1013
#define ERROR_WRONG_FORMAT -1014
#define ERROR_DUP_SYM -1015
#define ERROR_NOT_END -1016
#define ERROR_DEC_RANGE -1017
#define ERROR_DEC_FORMAT -1018
#define ERROR_NOT_ASM -1019	
#define ERROR_NOT_SYM -1020
#define ERROR_ADDR_MODE -1021
#define ERROR_NOT_OBJ -1022
#define ERROR_FILE_SEEK -1023
#define ERROR_LINK_CNT -1024
#define ERROR_MDF_OVERFLOW -1025
#define ERROR_NOT_INSTRUCT -1026
#define FLAG_EMPTY -2000

#define FILE_BUFFER_SIZE 1000
#define INSTRUCTION_BUFFER 100
#define INPUT_SIZE 100
#define _MAX_PATH 260
#define HASH_SIZE 20
#define FORMAT_SIZE 5
#define MEM_SIZE 65536*16
#define MNEMONIC_SIZE 30
#define SYMBOL_SIZE 30
#define OBJCODE_SIZE 30
#define RECODE_SIZE 100
#define _MAX_LINK 3
#define _MAX_REFER 256
#define BP_SIZE 1000
#endif
