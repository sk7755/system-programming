#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "main.h"

extern int break_point[];

void cmd_bp_show();
void cmd_bp_clear();
void cmd_bp_set(int value);
int cmp_increasing(const void* a,const void* b);
int check_bp(int value);

#endif
