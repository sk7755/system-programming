#ifndef __ASSEMBLE_H__
#define __ASSEMBLE_H__

#include "main.h"
#include "hash.h"
#include "argument.h"

typedef struct symbol_node* snptr;

typedef struct symbol_node
{
	char symbol[SYMBOL_SIZE];
	int loc;
}symbol_node;

typedef struct instruction* inptr;

typedef struct instruction
{
	char *comment;
	char *label;
	char *directive;
	char *mnemonic;
	char *operand;
	int loc;
	int line;
	inptr link;
}instruction;

typedef struct object_code
{
	int code;
	int format;
}object_code;
//in next project, create file io source code

int cmd_assemble(char filename[]);
void cmd_symbol();
int read_line(FILE* fp, char buffer[], char tmp_buffer[], int *line);
inptr make_instruction(char arg[], int line);
void clear_intermediate();
void add_symbol(char symbol[], int loc);
snptr find_symbol(char symbol[]);
void add_instruction(inptr pnew);
int assemble_pass1(FILE *asm_fp);
int assemble_pass2(FILE* list_fp, FILE* obj_fp);
int make_obj(inptr cur, object_code *obj);
int obj_to_str(object_code obj, char str[]);
void write_list_line(FILE *fp, inptr cur, char obj[]);
void write_obj_line(FILE *fp, char type, int record_start, char record[]);
int is_directive(char directive[]);
int is_mnemonic(char mnemonic[]);
int is_register(char reg[]);
int cmp_string(void* a, void* b);
hnptr* get_sym_table_addr();

#endif