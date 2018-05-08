#include "simulator.h"

unsigned int reg[16] = { 0, };
int immediate = 1;

int cmd_run()
{
	reg[PC] = prog_addr;

	int flag = 0;
	do
	{
		int cur_addr = reg[PC];
		unsigned char format = (memory[cur_addr] & 0xF0) / 0x10;

		if (check_bp(reg[PC]))
			flag = 1;

		if (0xC == format && format == 0xF)
		{
			reg[PC] += 1;
			instruction_f1(cur_addr);
		}
		else if (0x9 <= format && format <= 0xB)
		{
			reg[PC] += 2;
			instruction_f2(cur_addr);
		}
		else
		{
			int extend = 0;
			if (memory[cur_addr + 1] & 0x10)
			{
				reg[PC] += 4;
				extend = 1;
			}
			else
				reg[PC] += 3;
			instruction_f34(cur_addr, extend);
		}

		if (flag)
		{
			show_register();
			printf("End program to break point.\n");
			prog_addr = reg[PC];
			return 1;
		}
	}while(reg[PC] != 0);
	show_register();
	printf("End totally program.\n");
	printf("Initialize Registers\n");
	for(int i = 0 ;i<16;i++)
		reg[i] = 0;

	prog_addr = org_prog_addr;
	return 0;
}

void show_register()
{
	for (int i = 0; i < 16; i++)
		reg[i] = reg[i] & 0xFFFFFF;

	printf("A : %06X X : %06X\n", reg[A], reg[X]);
	printf("L : %06X PC: %06X\n", reg[L], reg[PC]);
	printf("B : %06X S : %06X\n", reg[B], reg[S]);
	printf("T : %06X\n", reg[T]);
}

int instruction_f34(int cur_addr, int extend)
{
	unsigned int opcode = memory[cur_addr] & 0xFC;

	unsigned int flag = ((memory[cur_addr] & 0x03) << 8) | (memory[cur_addr + 1]);

	unsigned int tar_addr = ((memory[cur_addr + 1] & 0x0F) << 8) | memory[cur_addr + 2];


	if (extend)
		tar_addr = (tar_addr << 8) | memory[cur_addr + 3];	//20bit

	

	if (flag & 0x080)		//indexed mode
		tar_addr += reg[X];

	if ((flag & 0x040) && !(flag & 0x020))		//base relative mode
	{
		if(tar_addr & 0x800)
			tar_addr = tar_addr | 0xFFF000;
		tar_addr += reg[B];
	}
	if (!(flag & 0x040) && (flag & 0x020))		//pc relative mode
	{
		if (tar_addr & 0x800)
			tar_addr = tar_addr | 0xFFF000;
		tar_addr += reg[PC];
	}
	if (flag & 0x200 && !(flag & 0x100))	//indirect mode if not simple mode
	{
		tar_addr = tar_addr & 0xFFFFF;	//rounding;
		tar_addr = (memory[tar_addr] << 16) | (memory[tar_addr + 1] << 8) | memory[tar_addr + 2];
	}

	immediate = 0;
	if (!(flag & 0x200) && flag & 0x100)
		immediate = 1;

	if (opcode == 0x18)
		return inst_add(tar_addr);
	else if (opcode == 0x58)
		return inst_addf(tar_addr);
	else if (opcode == 0x40)
		return inst_and(tar_addr);
	else if (opcode == 0x28)
		return inst_comp(tar_addr);
	else if (opcode == 0x88)
		return inst_compf(tar_addr);
	else if (opcode == 0x24)
		return inst_div(tar_addr);
	else if (opcode == 0x64)
		return inst_divf(tar_addr);
	else if (opcode == 0x3C)
		return inst_j(tar_addr);
	else if (opcode == 0x30)
		return inst_jeq(tar_addr);
	else if (opcode == 0x34)
		return inst_jgt(tar_addr);
	else if (opcode == 0x38)
		return inst_jlt(tar_addr);
	else if (opcode == 0x48)
		return inst_jsub(tar_addr);
	else if (opcode == 0x00)
		return inst_lda(tar_addr);
	else if (opcode == 0x68)
		return inst_ldb(tar_addr);
	else if (opcode == 0x50)
		return inst_ldch(tar_addr);
	else if (opcode == 0x70)
		return inst_ldf(tar_addr);
	else if (opcode == 0x08)
		return inst_ldl(tar_addr);
	else if (opcode == 0x6C)
		return inst_lds(tar_addr);
	else if (opcode == 0x74)
		return inst_ldt(tar_addr);
	else if (opcode == 0x04)
		return inst_ldx(tar_addr);
	else if (opcode == 0xD0)
		return inst_lps(tar_addr);
	else if (opcode == 0x20)
		return inst_mul(tar_addr);
	else if (opcode == 0x60)
		return inst_mulf(tar_addr);
	else if (opcode == 0x44)
		return inst_or(tar_addr);
	else if (opcode == 0xD8)
		return inst_rd(tar_addr);
	else if (opcode == 0x4C)
		return inst_rsub(tar_addr);
	else if (opcode == 0xEC)
		return inst_ssk(tar_addr);
	else if (opcode == 0x0C)
		return inst_sta(tar_addr);
	else if (opcode == 0x78)
		return inst_stb(tar_addr);
	else if (opcode == 0x54)
		return inst_stch(tar_addr);
	else if (opcode == 0x80)
		return inst_stf(tar_addr);
	else if (opcode == 0xD4)
		return inst_sti(tar_addr);
	else if (opcode == 0x14)
		return inst_stl(tar_addr);
	else if (opcode == 0x7C)
		return inst_sts(tar_addr);
	else if (opcode == 0xE8)
		return inst_stsw(tar_addr);
	else if (opcode == 0x84)
		return inst_stt(tar_addr);
	else if (opcode == 0x10)
		return inst_stx(tar_addr);
	else if (opcode == 0x1C)
		return inst_sub(tar_addr);
	else if (opcode == 0x5C)
		return inst_subf(tar_addr);
	else if (opcode == 0xE0)
		return inst_td(tar_addr);
	else if (opcode == 0x2C)
		return inst_tix(tar_addr);
	else if (opcode == 0xDC)
		return inst_wd(tar_addr);
	else
	{
		error_print(ERROR_NOT_INSTRUCT);
		return 0;
	}
}

int load_word(int addr)
{
	if (immediate)
		return addr;
	addr = addr & 0xFFFFF;
	return (memory[addr] << 16) + (memory[addr + 1] << 8) + (memory[addr + 2]);
}

int load_byte(int addr)
{
	if (immediate)
		return addr;

	addr = addr & 0xFFFFF;
	return memory[addr];
}

int inst_add(int tar_addr)
{
	//A <- A + m(0~2)
	reg[A] += load_word(tar_addr);
	return 1;
}

int inst_addf(int tar_addr)	//float instruction need to 48bit memory
{
	//F <- F + m(0~5)
	return 1;
}

int inst_and(int tar_addr)
{
	//A <- A & m(0~2)
	reg[A] = reg[A] & load_word(tar_addr);
	return 1;
}

int inst_comp(int tar_addr)
{
	//A : m(0~2)

	unsigned int value = load_word(tar_addr);
	if (A < value)
		reg[SW] = LT;
	else if (A > value)
		reg[SW] = GT;
	else
		reg[SW] = EQ;

	return 1;
}
int inst_compf(int tar_addr)
{
	//F : m(0~5)

	return 1;
}
int inst_div(int tar_addr)
{
	//A <- A / m(0~2)

	reg[A] = reg[A] / load_word(tar_addr);

	return 1;
}
int inst_divf(int tar_addr)
{
	//F <- F / m(0~5)

	return 1;
}
int inst_j(int tar_addr)
{
	//PC <- m

	reg[PC] = tar_addr & 0xFFFFF;

	return 1;
}
int inst_jeq(int tar_addr)
{
	//if cc set to =, PC <- m
	if (reg[SW] == EQ)
		reg[PC] = tar_addr & 0xFFFFF;

	return 1;
}
int inst_jgt(int tar_addr)
{
	//if cc set to >, PC <- m

	if (reg[SW] == GT)
		reg[PC] = tar_addr & 0xFFFFF;

	return 1;
}
int inst_jlt(int tar_addr)
{
	//if cc set to <, PC <- m

	if (reg[SW] == LT)
		reg[PC] = tar_addr & 0xFFFFF;

	return 1;
}
int inst_jsub(int tar_addr)
{
	//L <- PC ; PC <- m

	reg[L] = reg[PC];
	reg[PC] = tar_addr & 0xFFFFF;

	return 1;
}

//A <- m(0~2)
int inst_lda(int tar_addr)
{
	reg[A] = load_word(tar_addr);

	return 1;
}

//B <- m(0~2)
int inst_ldb(int tar_addr)
{
	reg[B] = load_word(tar_addr);
	return 1;
}

//A[rightmost byte] <- (m)
int inst_ldch(int tar_addr)
{
	reg[A] = load_byte(tar_addr);

	return 1;
}

//F <- m(0~5)
int inst_ldf(int tar_addr)
{
	return 1;
}

//L <- m(0~2)
int inst_ldl(int tar_addr)
{
	reg[L] = load_word(tar_addr);

	return 1;
}

//S <- m(0~2)
int inst_lds(int tar_addr)
{
	reg[S] = load_word(tar_addr);

	return 1;
}

//T <- m(0~2)
int inst_ldt(int tar_addr)
{
	reg[T] = load_word(tar_addr);

	return 1;
}

//X <- m(0~2)
int inst_ldx(int tar_addr)
{
	reg[X] = load_word(tar_addr);

	return 1;
}

//Load processor status from information beginning at address m
int inst_lps(int tar_addr)
{
	return 1;
}

//A <- A * m(0~2)
int inst_mul(int tar_addr)
{
	reg[A] *= load_word(tar_addr);

	return 1;
}

//F <- F * m(0~2)
int inst_mulf(int tar_addr)
{
	return 1;
}

//A <- A | m(0~2)
int inst_or(int tar_addr)
{
	reg[A] = reg[A] | load_word(tar_addr);

	return 1;
}

//A[rightmost byte] <- data from device specified by (m)
int inst_rd(int tar_addr)
{
	reg[SW] = LT;
	return 1;
}

//PC <- L
int inst_rsub(int tar_addr)
{
	reg[PC] = reg[L];

	return 1;
}

//protection key for address m <- A
int inst_ssk(int tar_addr)
{
	return 1;
}

//m(0~2) <- A
int inst_sta(int tar_addr)
{
	memory[tar_addr] = (reg[A] & 0xFF0000) / 0x10000;
	memory[tar_addr + 1] = (reg[A] & 0x00FF00) / 0x100;
	memory[tar_addr + 2] = reg[A] & 0x0000FF;

	return 1;
}

//m(0~2) <- B
int inst_stb(int tar_addr)
{
	tar_addr = tar_addr & 0xFFFFF;
	memory[tar_addr] = (reg[B] & 0xFF0000) / 0x10000;
	memory[tar_addr + 1] = (reg[B] & 0x00FF00) / 0x100;
	memory[tar_addr + 2] = reg[B] & 0x0000FF;

	return 1;
}

//m <- A [rightmost byte]
int inst_stch(int tar_addr)
{
	tar_addr = tar_addr & 0xFFFFF;
	memory[tar_addr] = reg[A] & 0x0000FF;

	return 1;
}

//m(0~5) <- F
int inst_stf(int tar_addr)
{
	return 1;
}

//Interval timer value <- m(0~2)
int inst_sti(int tar_addr)
{
	return 1;
}
//m(0~2) <- L
int inst_stl(int tar_addr)
{
	tar_addr = tar_addr & 0xFFFFF;
	memory[tar_addr] = (reg[L] & 0xFF0000) / 0x10000;
	memory[tar_addr + 1] = (reg[L] & 0x00FF00) / 0x100;
	memory[tar_addr + 2] = reg[L] & 0x0000FF;

	return 1;
}

//m(0~2) <- S
int inst_sts(int tar_addr)
{
	tar_addr = tar_addr & 0xFFFFF;
	memory[tar_addr] = (reg[S] & 0xFF0000) / 0x10000;
	memory[tar_addr + 1] = (reg[S] & 0x00FF00) / 0x100;
	memory[tar_addr + 2] = reg[S] & 0x0000FF;

	return 1;
}

//m(0~2) <- SW
int inst_stsw(int tar_addr)
{
	tar_addr = tar_addr & 0xFFFFF;
	memory[tar_addr] = (reg[SW] & 0xFF0000) / 0x10000;
	memory[tar_addr + 1] = (reg[SW] & 0x00FF00) / 0x100;
	memory[tar_addr + 2] = reg[SW] & 0x0000FF;

	return 1;
}

//m(0~2) <- T
int inst_stt(int tar_addr)
{
	tar_addr = tar_addr & 0xFFFFF;
	memory[tar_addr] = (reg[T] & 0xFF0000) / 0x10000;
	memory[tar_addr + 1] = (reg[T] & 0x00FF00) / 0x100;
	memory[tar_addr + 2] = reg[T] & 0x0000FF;

	return 1;
}

//m(0~2) <- X
int inst_stx(int tar_addr)
{
	tar_addr = tar_addr & 0xFFFFF;
	memory[tar_addr] = (reg[X] & 0xFF0000) / 0x10000;
	memory[tar_addr + 1] = (reg[X] & 0x00FF00) / 0x100;
	memory[tar_addr + 2] = reg[X] & 0x0000FF;

	return 1;
}

//A <- A - m(0~2)
int inst_sub(int tar_addr)
{
	reg[A] -= load_word(tar_addr);
	return 1;
}

//F <- F - m(0~5)
int inst_subf(int tar_addr)
{
	return 1;
}

//Test device specified by m
int inst_td(int tar_addr)
{
	reg[SW] = LT;
	return 1;
}

//X <- X + 1 ; X : m(0~2)
int inst_tix(int tar_addr)
{
	unsigned int value = load_word(tar_addr);
	reg[X]++;
	if (reg[X] < value)
		reg[SW] = LT;
	else if (reg[X] > value)
		reg[SW] = GT;
	else
		reg[SW] = EQ;
	return 1;
}

//Device specified by m <- A[rightmost byte]
int inst_wd(int tar_addr)
{
	return 1;
}

int instruction_f2(int cur_addr)
{
	if (memory[cur_addr] == 0x90)
		return inst_addr(cur_addr+1);
	else if (memory[cur_addr] == 0xB4)
		return inst_clear(cur_addr + 1);
	else if (memory[cur_addr] == 0xA0)
		return inst_compr(cur_addr + 1);
	else if (memory[cur_addr] == 0x9C)
		return inst_divr(cur_addr + 1);
	else if (memory[cur_addr] == 0x98)
		return inst_mulr(cur_addr + 1);
	else if (memory[cur_addr] == 0xAC)
		return inst_rmo(cur_addr + 1);
	else if (memory[cur_addr] == 0xA4)
		return inst_shiftl(cur_addr + 1);
	else if (memory[cur_addr] == 0xA8)
		return inst_shiftr(cur_addr + 1);
	else if (memory[cur_addr] == 0x94)
		return inst_subr(cur_addr + 1);
	else if (memory[cur_addr] == 0xB0)
		return inst_svc(cur_addr + 1);
	else if (memory[cur_addr] == 0xB8)
		return inst_tixr(cur_addr + 1);
	else
	{
		error_print(ERROR_NOT_INSTRUCT);
		return 0;
	}
	
}

int inst_addr(int cur_addr)
{
	//r2 <- (r2) + (r1)
	int r1 = memory[cur_addr] / 0x10;
	int r2 = memory[cur_addr] % 0x10;

	reg[r2] = reg[r2] + reg[r1];
	return 1;
}
int inst_clear(int cur_addr)
{
	//r1 <- 0
	int r1 = memory[cur_addr] / 0x10;
	int r2 = memory[cur_addr] % 0x10;

	if (r2 != 0)	//instruction error
		;

	reg[r1] = 0;
	return 1;
}
int inst_compr(int cur_addr)
{
	//(r1) : (r2)
	int r1 = memory[cur_addr] / 0x10;
	int r2 = memory[cur_addr] % 0x10;

	if (reg[r1] < reg[r2])
		reg[SW] = LT;
	else if (reg[r1] > reg[r2])
		reg[SW] = GT;
	else
		reg[SW] = EQ;

	return 1;
}
int inst_divr(int cur_addr)
{
	//r2 <- r2 / r1
	int r1 = memory[cur_addr] / 0x10;
	int r2 = memory[cur_addr] % 0x10;

	reg[r2] = reg[r2] / reg[r1];

	return 1;
}
int inst_mulr(int cur_addr)
{
	//r2 <- r2 * r1
	int r1 = memory[cur_addr] / 0x10;
	int r2 = memory[cur_addr] % 0x10;

	reg[r2] = reg[r2] * reg[r1];

	return 1;
}
int inst_rmo(int cur_addr)
{
	//r2 <- r1
	int r1 = memory[cur_addr] / 0x10;
	int r2 = memory[cur_addr] % 0x10;

	reg[r2] = reg[r1];
	
	return 1;
}
int inst_shiftl(int cur_addr)
{
	//r1 <- (r1) << left circular shift n bits
	int r1 = memory[cur_addr] / 0x10;
	int r2 = memory[cur_addr] % 0x10;
	r2++;

	reg[r1] = (reg[r1] << r2) | (reg[r1] >> (32 - r2));

	return 1;

}
int inst_shiftr(int cur_addr)
{
	//r1 <- (r1) >> right arithmetic shift n bits
	int r1 = memory[cur_addr] / 0x10;
	int r2 = memory[cur_addr] % 0x10;
	r2++;

	int sign = 0;

	if (reg[r1] & 0x800000)
		sign = 1;

	reg[r1] = (reg[r1] >> r2) | ((((0x1 << r2) - 1) << (32 - r2)) * sign);

	return 1;
}
int inst_subr(int cur_addr)
{
	//r2 <- r2 - r1
	int r1 = memory[cur_addr] / 0x10;
	int r2 = memory[cur_addr] % 0x10;

	reg[r2] = reg[r2] - reg[r1];

	return 1;
}
int inst_svc(int cur_addr)
{
	//generate svc interrupt
	return 1;
}


//X <- (X) + 1; (X) : (r1)
int inst_tixr(int cur_addr)
{
	int r1 = memory[cur_addr] / 0x10;

	reg[X]++;

	if (reg[X] < reg[r1])
		reg[SW] = LT;
	else if (reg[X] > reg[r1])
		reg[SW] = GT;
	else
		reg[SW] = EQ;

	return 1;
}

int instruction_f1(int cur_addr)
{
	if (memory[cur_addr] == 0xC4)
		return inst_fix();
	else if (memory[cur_addr] == 0xC0)
		return inst_float();
	else if (memory[cur_addr] == 0xF4)
		return inst_hio();
	else if (memory[cur_addr] == 0xC8)
		return inst_norm();
	else if (memory[cur_addr] == 0xF0)
		return inst_sio();
	else if (memory[cur_addr] == 0xF8)
		return inst_tio();
	else
	{
		error_print(ERROR_NOT_INSTRUCT);
		return 0;
	}
}

int inst_fix()
{
	//A <- F(convert to integer)
	return 1;
}

int inst_float()
{
	//F <- A(convert to float)
	return 1;
}

int inst_hio()
{
	//Halt I/O channel number(A)
	return 1;
}

int inst_norm()
{
	//F <- (F) [nomalized]
	return 1;
}

int inst_sio()
{
	//start I/O channel number(A)
	//address of channel program is given by (S)
	return 1;
}

int inst_tio()
{
	//Test I/O channel number (A)
	return 1;
}
