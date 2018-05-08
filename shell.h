#ifndef __SHELL_H__
#define __SHELL_H__

#include "main.h"
#include "hash.h"
#include "opcode.h"
#include "assemble.h"

typedef struct history_node* hinptr;

void add_history(char arg[]);
void clear_history();
void cmd_help();
void cmd_history();
int cmd_dir();
int cmd_type(char filename[]);
void cmd_quit();

#endif