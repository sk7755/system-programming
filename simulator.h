#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__

#include "main.h"
#include "linkloader.h"
#include "debug.h"
#include "memory.h"

extern unsigned int reg[16];

enum { A = 0, X = 1, L = 2, PC = 8, SW = 9, B = 3, S = 4, T = 5, F = 6 };
enum { LT, GT, EQ };
typedef enum addressing { EMPTY, IMMEDIATE, INDIRECT, INDEXED, RELATIVE_B, RELATIVE_PC, DIRECT, EXTEND }addressing;

typedef struct ADDR_MODE
{
	addressing m[4];
}ADDR_MODE;

int cmd_run();
void show_register();

int instruction_f1(int cur_addr);
int inst_fix();
int inst_float();
int inst_hio();
int inst_norm();
int inst_sio();
int inst_tio();

int instruction_f2(int cur_addr);
int inst_addr(int cur_addr);
int inst_clear(int cur_addr);
int inst_compr(int cur_addr);
int inst_divr(int cur_addr);
int inst_mulr(int cur_addr);
int inst_rmo(int cur_addr);
int inst_shiftl(int cur_addr);
int inst_shiftr(int cur_addr);
int inst_subr(int cur_addr);
int inst_svc(int cur_addr);
int inst_tixr(int cur_addr);

int instruction_f34(int cur_addr, int extend);
int load_word(int addr);
int load_byte(int addr);
int inst_add(int tar_value);
int inst_addf(int tar_value);
int inst_and(int tar_value);
int inst_comp(int tar_value);
int inst_compf(int tar_value);
int inst_div(int tar_value);
int inst_divf(int tar_value);
int inst_j(int tar_value);
int inst_jeq(int tar_value);
int inst_jgt(int tar_value);
int inst_jlt(int tar_value);
int inst_jsub(int tar_value);
int inst_lda(int tar_value);
int inst_ldb(int tar_value);
int inst_ldch(int tar_value);
int inst_ldf(int tar_value);
int inst_ldl(int tar_value);
int inst_lds(int tar_value);
int inst_ldt(int tar_value);
int inst_ldx(int tar_value);
int inst_lps(int tar_value);
int inst_mul(int tar_value);
int inst_mulf(int tar_value);
int inst_or(int tar_value);
int inst_rd(int tar_value);
int inst_rsub(int tar_value);
int inst_ssk(int tar_value);
int inst_sta(int tar_value);
int inst_stb(int tar_value);
int inst_stch(int tar_value);
int inst_stf(int tar_value);
int inst_sti(int tar_value);
int inst_stl(int tar_value);
int inst_sts(int tar_value);
int inst_stsw(int tar_value);
int inst_stt(int tar_value);
int inst_stx(int tar_value);
int inst_sub(int tar_value);
int inst_subf(int tar_value);
int inst_td(int tar_value);
int inst_tix(int tar_value);
int inst_wd(int tar_value);
#endif
