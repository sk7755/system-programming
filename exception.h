#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include "main.h"
#include "assemble.h"
#include "hash.h"

void error_print(int error);
void asm_error(int error, int line);

#endif