#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "main.h"

extern unsigned char memory[MEM_SIZE];

int cmd_dump(int start, int end);
int cmd_edit(int addr, int value);
int cmd_fill(int start, int end, int value);
void cmd_reset();

#endif