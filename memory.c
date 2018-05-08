#include "memory.h"

unsigned char memory[MEM_SIZE] = { 0, };
int last_addr = -1;

//execute dump command
//show memory status from start address to end address
//print the first address in line and each value with ASCII code
//if start is greater than end, return error number else return 1
int cmd_dump(int start, int end)
{
	if (start == FLAG_EMPTY)
		start = last_addr + 1;

	if (start == MEM_SIZE)
		start = 0;

	if (end == FLAG_EMPTY)
		end = start + 159;

	if (start > end)
		return ERROR_SE_RANGE;
	if (end >= MEM_SIZE)
		end = MEM_SIZE - 1;

	for (int i = start / 16; i <= end / 16; i++)
	{
		printf("%05X ", i * 0x10);

		for (int j = 0; j<16; j++)
		{
			int cur = i * 0x10 + j;
			if (start <= cur  && cur <= end)
				printf("%02X ", memory[cur]);
			else
				printf("   ");
		}
		printf("; ");

		for (int j = 0; j<16; j++)
		{
			int cur = i * 0x10 + j;
			if (start <= cur && cur <= end && 0x20 <= memory[cur] && memory[cur] <= 0x7E)
				printf("%c", memory[cur]);
			else
				printf(".");
		}
		printf("\n");
	}
	last_addr = end;

	return 1;
}

//execute edit command
//edit original value in the address to the given value
//if value is out of range return error number else return 1
int cmd_edit(int addr, int value)
{
	if (value > 0xFF)
		return ERROR_VALUE_RANGE;

	memory[addr] = value;
	return 1;
}

//execute fill command
//fill memory with given value from the start address to the end address
//if start is greater than end or value is out of range return error number else return 1
int cmd_fill(int start, int end, int value)
{
	if (value > 0xFF)
		return ERROR_VALUE_RANGE;

	if (start > end)
		return ERROR_SE_RANGE;

	for (int i = start; i <= end; i++)
		memory[i] = value;

	return 1;
}

//execute reset command
//reset all memory by zero
void cmd_reset()
{
	for (int i = 0; i<MEM_SIZE; i++)
		memory[i] = 0;
}