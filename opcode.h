#ifndef __OPCODE_H__
#define __OPCODE_H__

#include "main.h"
#include "hash.h"
#include "exception.h"
#include "argument.h"

typedef struct opcode_node* onptr;

typedef struct opcode_node
{
	char mnemonic[MNEMONIC_SIZE];
	int opcode;
	char format[FORMAT_SIZE];
}opcode_node;

int read_opcode_file();
onptr find_mnemonic(char mnemonic[]);
void cmd_opcodelist();
int cmd_opcode_mnemonic(char mnemonic[]);
hnptr* get_op_table_addr();

#endif
