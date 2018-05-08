#include "debug.h"

int break_point[BP_SIZE] = { 0, };
int bp_cnt = 0;

//show all break point
void cmd_bp_show()
{
	if (bp_cnt == 0)
	{
		printf("There not exist break point\n");
		return ;
	}

	printf("breakpoint\n");
	printf("----------\n");
	for (int i = 0; i < bp_cnt; i++)
	{
		printf("%05X\n", break_point[i]);
	}

	return;
}

//clear all break point
void cmd_bp_clear()
{
	bp_cnt = 0;
	printf("[ok] clear all breakpoints\n");
}

//set break point
void cmd_bp_set(int value)
{
	if (!check_bp(value))
	{
		break_point[bp_cnt++] = value;
		qsort(break_point, bp_cnt, sizeof(int), cmp_increasing);
	}
	printf("[ok] create breakpoint %05X\n", value);
	
}

//a purpose of the function is to sort break point
int cmp_increasing(const void* a,const void* b)
{
	return *(int*)a > *(int*)b;
}

//check if value is break point
//if true, return 1 else return 0
int check_bp(int value)
{
	int left = 0, right = bp_cnt - 1;

	while (left <= right)
	{
		int mid = left + right / 2;

		if (break_point[mid] == value)
			return 1;
		else if (break_point[mid] < value)
			left = mid + 1;
		else
			right = mid - 1;
	}
	return 0;
}
