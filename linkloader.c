#include "linkloader.h"

int prog_addr = 0;	//default 0x00
int org_prog_addr = 0;
int exe_addr = 0;
hnptr esym_table[HASH_SIZE] = { 0, };
LOAD_MAP* load_map = NULL;

int cmd_loader(char filename[_MAX_LINK][INPUT_SIZE], int file_cnt)
{
	FILE* fp[_MAX_LINK] = { 0, };

	for (int i = 0; i < HASH_SIZE; i++)
		esym_table[i] = NULL;

	load_map = (LOAD_MAP*)malloc(sizeof(*load_map));
	for (int i = 0; i < _MAX_LINK; i++)
	{
		load_map->head[i] = NULL;
		load_map->rear[i] = NULL;
		load_map->prog_length[i] = 0;
	}
	load_map->link_cnt = file_cnt;

	for (int i = 0; i < file_cnt; i++)
	{
		int len = strlen(filename[i]);
		if (len <= 4 || strcmp(filename[i] + len - 4, ".obj") != 0)		//NOT obj file
		{
			error_print(ERROR_NOT_OBJ);
			return 0;
		}

		fp[i] = fopen(filename[i], "r");
		if (fp[i] == NULL)
		{
			error_print(ERROR_FILE_OPEN);
			return 0;
		}
	}		//file open

	int success = 1;
	if (loader_pass1(fp,file_cnt))
	{
		//init file buf
		for (int i = 0; i < file_cnt; i++)
		{
			if (fseek(fp[i], 0L, SEEK_SET) != 0)
			{
				error_print(ERROR_FILE_SEEK);
				success = 0;
				break;
			}
		}
		
		if (success && loader_pass2(fp,file_cnt))
		{
			print_load_map();
		}
		else
			success = 0;
	}
	else
		success = 0;
	
	clear_hash(esym_table);
	free(load_map);
	load_map = NULL;

	for (int i = 0; i < file_cnt; i++)
	{
		if (fclose(fp[i]) == EOF)
		{
			error_print(ERROR_FILE_CLOSE);
			success = 0;
		}
	}
	//file close

	return success;
}

int loader_pass1(FILE* fp[], int file_cnt)
{
	int cs_addr = prog_addr;	//control section addr
	int cs_length = 0;
	char buffer[FILE_BUFFER_SIZE];
	char parse[FILE_BUFFER_SIZE];
	char record_type = 0;

	for (int i = 0; i < file_cnt; i++)
	{
		record_type = obj_read_line(fp[i], buffer);	//read header record
		strncpy(parse, buffer + 13, 6);		//program length. HPROGA 000000(000063)
		parse[6] = '\0';

		cs_length = hex_to_int(parse);
		load_map->prog_length[i] = cs_length;

		strncpy(parse, buffer + 1, 6);		//program name
		parse[6] = '\0';

		if (find_esymbol(parse))
		{
			error_print(ERROR_DUP_SYM);
			return 0;
			//prgram name duplicate external symbol error
		}
		add_esymbol(parse, cs_addr,i);

		while (record_type = obj_read_line(fp[i], buffer))
		{
			if (record_type == 'E')
				break;

			if (record_type == 'D')
			{
				int len = strlen(buffer);

				int cursor = 1;
				while (cursor < len)
				{
					strncpy(parse, buffer + cursor + 6, 6);
					parse[6] = '\0';
					int loc = hex_to_int(parse);	//normal input assume
					strncpy(parse, buffer + cursor, 6);
					parse[6] = '\0';

					if (find_esymbol(parse))
					{
						error_print(ERROR_DUP_SYM);
						return 0;
					}
					add_esymbol(parse,cs_addr+loc, i);
					cursor += 12;
				}
			}
		}
		cs_addr += cs_length;
	}
	if (cs_addr > 0x100000)
	{
		error_print(ERROR_MEM_BOUND);
		return 0;
	}

	return 1;
}

int loader_pass2(FILE* fp[], int file_cnt)
{
	int cs_addr = prog_addr;
	exe_addr = prog_addr;		//exe_addr default
	int cs_length = 0;
	char buffer[FILE_BUFFER_SIZE];
	char parse[FILE_BUFFER_SIZE];
	char record_type = 0;

	for (int i = 0; i < file_cnt; i++)
	{
		esnptr refer[_MAX_REFER + 1] = { 0, };
		refer[1] = load_map->head[i];			//refer[1] = program esymbol

		cs_length = load_map->prog_length[i];

		while (record_type = obj_read_line(fp[i], buffer))
		{
			if (record_type == 'E')
				break;

			if (record_type == 'R')
			{
				int len = strlen(buffer);
				int cursor = 1;
				while (cursor < len)
				{
					strncpy(parse, buffer + cursor, 2);
					parse[2] = '\0';
					int idx = hex_to_int(parse);

					strncpy(parse, buffer + cursor + 2, 6);
					int tmp = 0;
					while((tmp = strlen(parse)) < 6)
					{
						parse[tmp] = ' ';
						parse[tmp+1] = '\0';
					}

					parse[6] = '\0';

					esnptr esymbol = find_esymbol(parse);

					if (esymbol == NULL)
					{
						error_print(ERROR_NOT_SYM);
						return 0;
					}
					refer[idx] = esymbol;
					cursor += 8;
				}
			}

			if (record_type == 'T')
			{
				strncpy(parse, buffer+1, 6);
				parse[6] = '\0';
				int start_addr = hex_to_int(parse) + cs_addr;	//set start_addr
				
				strncpy(parse, buffer + 7, 2);
				parse[2] = '\0';
				int record_len = hex_to_int(parse);				//set record length

				int cursor = 9;
				for (int j = 0; j < record_len; j++,cursor += 2)
				{
					strncpy(parse, buffer + cursor, 2);
					parse[2] = '\0';
					int value = hex_to_int(parse);

					memory[start_addr + j] = value;
				}												//write memory
			}

			if (record_type == 'M')
			{
				strncpy(parse, buffer + 1, 6);
				parse[6] = '\0';
				int tar_addr = hex_to_int(parse) + cs_addr;

				strncpy(parse, buffer + 7, 2);
				parse[2] = '\0';
				int mdf_len = hex_to_int(parse);

				int sign = 0;	//sign = 0 is plus
				strncpy(parse, buffer + 9, 1);
				if (parse[0] == '-')
					sign = 1;

				strncpy(parse, buffer + 10, 2);
				parse[2] = '\0';
				int idx = hex_to_int(parse);

				int mdf_value = 0;

				for (int j = 0; j < (mdf_len + 1) / 2; j++)
				{
					mdf_value *= 0x100;
					mdf_value += memory[tar_addr + j];
				}//memory load
				
				
				if (mdf_len % 2)		//mdf_len = 5
					mdf_value = mdf_value & 0x0FFFFF;
				
				if (sign)
					mdf_value -= refer[idx]->loc;
				else
					mdf_value += refer[idx]->loc;

				//do not check overflow

				if (mdf_len % 2)
				{
					memory[tar_addr] = memory[tar_addr] & 0xF0;
					memory[tar_addr] += (mdf_value & 0x0F0000) / 0x10000;
					tar_addr++;
				}

				for (int j = mdf_len/2 - 1; j >= 0 ; j--)
				{
					memory[tar_addr + j] = mdf_value % 0x100;
					mdf_value /= 0x100;
				}
			}
		}
		if (record_type == 'E')	//if normal exit, break by end record
		{
			int len = strlen(buffer);
			if (len > 1)	//set exe_addr
			{
				strncpy(parse, buffer + 1, 6);
				exe_addr = hex_to_int(parse) + cs_addr;
			}
		}
		cs_addr += cs_length;
	}
	return 1;
}

void cmd_progaddr(int value)
{
	prog_addr = value;
	org_prog_addr = value;
}
//read line from right obj file (assume that we read right obj file)
//return record type and interchange first character by spacebar from record line
//if read end of file, return 0
int obj_read_line(FILE* fp, char buffer[])
{
	if (fgets(buffer, FILE_BUFFER_SIZE, fp) == NULL)
		return 0;
	int ret = buffer[0];
	buffer[0] = ' ';
	int len = strlen(buffer);
	if(buffer[len-1] == '\n')
		buffer[len-1] = '\0';

	return ret;
}

void add_esymbol(char esymbol[], int loc, int idx)
{
	esymbol[6] = '\0';	//extern symbol size always be 6
	int hash_number = hash_function(esymbol);
	esnptr pnew = (esnptr)malloc(sizeof(*pnew));
	strcpy(pnew->symbol, esymbol);
	pnew->loc = loc;
	pnew->link = NULL;
	add_hash(esym_table, pnew, hash_number);
	add_load_map(pnew, idx);
}

void add_load_map(esnptr esym_node, int idx)
{
	if (load_map->head[idx] == NULL)
	{
		load_map->head[idx] = load_map->rear[idx] = esym_node;
		return;
	}
	(load_map->rear[idx])->link = esym_node;
	load_map->rear[idx] = esym_node;
}

esnptr find_esymbol(char esymbol[])
{
	int hash_number = hash_function(esymbol);

	for (hnptr p = esym_table[hash_number]; p; p = p->link)
	{
		esnptr esp = p->data;
		if (strcmp(esp->symbol, esymbol) == 0)
			return esp;
	}

	return NULL;
}

void print_load_map()
{
	printf("control      symbol       address         length\n");
	printf("section      name\n");
	printf("----------------------------------------------------\n");
	for (int i = 0; i < load_map->link_cnt; i++)
	{
		esnptr p = load_map->head[i];
		printf("%-13s%-13s%-16X%-13X\n",p->symbol," ",p->loc,load_map->prog_length[i]);
		
		for (p = p->link; p; p = p->link)
			printf("%-13s%-13s%-16X\n"," ", p->symbol, p->loc);
	}
	printf("----------------------------------------------------\n");
}
