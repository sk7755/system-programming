#include "exception.h"

//according to error parameter, print proper error message
//errors number are defined in header file
void error_print(int error)
{
	if (error == ERROR_OVER_ARG)
		printf("ERROR_OVER_ARG : over argument\n");
	if (error == ERROR_HEX_FORMAT)
		printf("ERROR_HEX_FORMAT : please write right positive hex code\n");
	if (error == ERROR_HEX_RANGE)
		printf("ERROR_HEX_RANGE : hex code can be range of 0x00000 ~ 0xFFFFF\n");
	if (error == ERROR_MEM_BOUND)
		printf("ERROR_MEM_BOUND : memory can't over 0xFFFFF\n");
	if (error == ERROR_LACK_ARG)
		printf("ERROR_LACK_ARG : lack argument\n");
	if (error == ERROR_SE_RANGE)
		printf("ERROR_SE_RANGE : start can't be greater than end\n");
	if (error == ERROR_VALUE_RANGE)
		printf("ERROR_VALUE_RANGE : wrong range of value\n");
	if (error == ERROR_NOT_FOUND)
		printf("ERROR_NOT_FOUND : can not find command\n");
	if (error == ERROR_FILE_OPEN)
		printf("ERROR_FILE_OPEN : fail to open file\n");
	if (error == ERROR_FILE_CLOSE)
		printf("ERROR_FILE_CLOSE : fail to close file\n");
	if (error == ERROR_NOT_MNEMONIC)
		printf("ERROR_NOT_MNEMONIC : can not find the mnemonic\n");
	if (error == ERROR_DIR_OPEN)
		printf("ERROR_DIR_OPEN : can not read information of current directory\n");
	if (error == ERROR_DIR_CLOSE)
		printf("ERROR_DIR_CLOSE : can not close current directory\n");
	if (error == ERROR_EMPTY_ASM)
		printf("ERROR_EMPTY_ASM : It is empty asm file\n");
	if (error == ERROR_WRONG_FORMAT)
		printf("ERROR_WRONG_FORMAT : please set right format\n");
	if (error == ERROR_DUP_SYM)
		printf("ERROR_DUP_SYM : duplicate symbol error\n");
	if (error == ERROR_NOT_END)
		printf("ERROR_NOT_END : can not find program end directive\n");
	if (error == ERROR_DEC_RANGE)
		printf("ERROR_DEC_RANGE : wrong range of decimal\n");
	if (error == ERROR_DEC_FORMAT)
		printf("ERROR_DEC_FORMAT : wrong format of decimal\n");
	if (error == ERROR_NOT_ASM)
		printf("ERROR_NOT_ASM : it is not asm file\n");
	if (error == ERROR_NOT_SYM)
		printf("ERROR_NOT_SYM : there is not the symbol in symbol table\n");
	if (error == ERROR_ADDR_MODE)
		printf("ERROR_ADDR_MODE : can not find right addressing mode\n");
	if (error == ERROR_NOT_OBJ)
		printf("ERROR_NOT_OBJ : it is not obj file\n");
	if (error == ERROR_FILE_SEEK)
		printf("ERROR_FILE_SEEK : can not move buffer cursor to start of file\n");
	if (error == ERROR_LINK_CNT)
		printf("ERROR_LINK_CNT : over maximum link count %d\n", _MAX_LINK);
	if (error == ERROR_MDF_OVERFLOW)
		printf("ERROR_MDF_OVERFLOW : value overflow by modification code\n");
	if (error == ERROR_NOT_INSTRUCT)
		printf("ERROR_NOT_INSTRUCT : can not run the no exist instruction\n");

}	

//error_print with error line
void asm_error(int error, int line)
{
	printf("LINE %d : ", line);
	error_print(error);
}