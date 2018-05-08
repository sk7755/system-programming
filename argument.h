#ifndef __ARGUMENT_H__
#define __ARGUMENT_H__

#include "main.h"
#include "exception.h"
#include "shell.h"
#include "memory.h"
#include "opcode.h"
#include "assemble.h"
#include "linkloader.h"
#include "debug.h"
#include "simulator.h"

void refine_argument(char arg[], char result[]);
int parsing_argument(char **arg, char **parse);
int hex_to_int(char hex[]);
int dec_to_int(char dec[]);
int int_to_hex(int num, char hex[], int length);
int is_help(char arg[], char tmp_arg[]);
int is_history(char arg[], char tmp_arg[]);
int is_dir(char arg[], char tmp_arg[]);
int is_type(char arg[], char tmp_arg[]);
int is_quit(char arg[]);
int is_dump(char arg[], char tmp_arg[]);
int is_edit(char arg[], char tmp_arg[]);
int is_fill(char arg[], char tmp_arg[]);
int is_reset(char arg[], char tmp_arg[]);
int is_opcodelist(char arg[], char tmp_arg[]);
int is_opcode_mnemonic(char arg[], char tmp_arg[]);
int is_assemble(char arg[], char tmp_arg[]);
int is_symbol(char arg[], char tmp_arg[]);
int is_loader(char arg[], char tmp_arg[]);
int is_progaddr(char arg[], char tmp_arg[]);
int is_bp(char arg[], char tmp_arg[]);
int is_run(char arg[], char tmp_arg[]);


#endif
