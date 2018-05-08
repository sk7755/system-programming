#ifndef __LINKLOADER_H__
#define __LINKLOADER_H__

#include "main.h"
#include "hash.h"
#include "argument.h"
#include "memory.h"

typedef struct esymbol_node* esnptr;

typedef struct esymbol_node
{
	char symbol[SYMBOL_SIZE];
	int loc;
	esnptr link;
}esymbol_node;

typedef struct LOAD_MAP
{
	esnptr head[_MAX_LINK];
	esnptr rear[_MAX_LINK];
	int prog_length[_MAX_LINK];
	int link_cnt;
}LOAD_MAP;

extern int prog_addr;
extern int org_prog_addr;

int cmd_loader(char filename[_MAX_LINK][INPUT_SIZE], int file_cnt);
int loader_pass1(FILE* fp[], int file_cnt);
int loader_pass2(FILE* fp[], int file_cnt);
int obj_read_line(FILE* fp, char buffer[]);
void add_esymbol(char esymbol[], int loc, int idx);
void add_load_map(esnptr esym_node, int idx);
esnptr find_esymbol(char esymbol[]);
void print_load_map();
void cmd_progaddr(int value);

#endif
