#include "assemble.h"

hnptr sym_table[HASH_SIZE] = { 0, };
inptr intermediate = NULL;
inptr rear = NULL;
int base_flag = 0;
int base = 0;
char program_name[7];
int program_length = 0;
int sym_table_num = 0;

int modification_table[1000];
int mdf_num = 0;

//assembler command
//assemble asm file to obj, lst file
//if error, print error and return 0
int cmd_assemble(char filename[])
{
	clear_hash(sym_table);
	sym_table_num =0;
	mdf_num = 0;

	base_flag = 0;
	base = 0;
	intermediate = NULL;
	rear = NULL;
	strcpy(program_name, "NONAME");
	program_length = 0;

	int len = strlen(filename);

	if (len <= 4 || strcmp(filename + len - 4, ".asm") != 0)		//NOT asm file
	{
		error_print(ERROR_NOT_ASM);
		return 0;
	}

	FILE* asm_fp = fopen(filename, "r");

	if (asm_fp == NULL)
	{
		error_print(ERROR_FILE_OPEN);
		return 0;
	}

	filename[len - 4] = '\0';
	strcat(filename, ".lst");
	FILE *list_fp = fopen(filename, "w");

	if (list_fp == NULL)
	{
		error_print(ERROR_FILE_OPEN);
		return 0;
	}

	clear_hash(sym_table);	//sym_table initialize
	sym_table_num = 0;

	filename[len - 4] = '\0';
	strcat(filename, ".obj");
	FILE *obj_fp = fopen(filename, "w");

	if (obj_fp == NULL)
	{
		error_print(ERROR_FILE_OPEN);
		return 0;
	}

	add_symbol("A", 0);
	add_symbol("X", 1);
	add_symbol("L", 2);
	add_symbol("B", 3);
	add_symbol("S", 4);
	add_symbol("T", 5);
	add_symbol("F", 6);
	add_symbol("PC", 8);
	add_symbol("SW", 9);

	int success = 0;
	if (assemble_pass1(asm_fp) && assemble_pass2(list_fp, obj_fp))
		success = 1;
	else
	{
		clear_hash(sym_table);
		char currentPath[_MAX_PATH];
		getcwd(currentPath, _MAX_PATH);
		strcat(currentPath,"/");
		strcat(currentPath,filename);
		remove(currentPath);
		currentPath[strlen(currentPath) - 4] = '\0';
		strcat(currentPath, ".lst");
		remove(currentPath);
		
	}
	clear_intermediate();

	if (fclose(asm_fp) == EOF)
	{
		error_print(ERROR_FILE_CLOSE);
		return 0;
	}

	if (fclose(list_fp) == EOF)
	{
		error_print(ERROR_FILE_CLOSE);
		return 0;
	}

	if (fclose(obj_fp) == EOF)
	{
		error_print(ERROR_FILE_CLOSE);
		return 0;
	}
	if(success)
	{
		filename[len - 4] = '\0';
		printf("output file : [%s.lst], [%s.obj]\n",filename,filename);
	}
	return success;
}

//symbol command
//print all symbol in symbol table
void cmd_symbol()
{
	if (sym_table_num == 0)
	{
		printf("symbol table is empty\n");
		return;
	}
	snptr *tmp = (snptr*)malloc(sizeof(*tmp) * sym_table_num);
	int k = 0;

	for (int i = 0; i < HASH_SIZE; i++)
	{
		for (hnptr p = sym_table[i]; p; p = p->link)
		{
			snptr sym = p->data;

			if (!is_register(sym->symbol))
				tmp[k++] = sym;
		}
	}
	for (int i = 0; i < k; i++)
	{
		for (int j = 0; j < k - i-1; j++)
		{
			if (cmp_string(tmp[j], tmp[j + 1]))
			{
				snptr t = tmp[j];
				tmp[j] = tmp[j + 1];
				tmp[j + 1] = t;
			}
		}
	}

	//qsort(tmp, k, sizeof(*tmp), cmp_string);
	
	for (int i = 0; i < k; i++)
	{
		printf("\t%s\t%04X\n", tmp[i]->symbol, tmp[i]->loc);
	}

	free(tmp);
	return;
}

//string comparison in increasing
int cmp_string(void* a,void* b)
{
	return strcmp(((snptr)a)->symbol , ((snptr)b)->symbol) < 0;
}

//assembler pass1 process
//allocate location address to each instruction
//add symbol to hash table
//create intermediate data
int assemble_pass1(FILE *asm_fp)
{
	char tmp_buffer[FILE_BUFFER_SIZE];
	char buffer[FILE_BUFFER_SIZE];
	char *arg;
	char *parse;
	int len;
	int line = 0;

	if(read_line(asm_fp, buffer, tmp_buffer,&line) == 0)
		asm_error(ERROR_EMPTY_ASM, line); //empty asm file

	inptr instruct = make_instruction(buffer, line);
	if (instruct == NULL)		//make_instruction error
		return 0;

	int locctr = 0;
	int start_addr = 0;

	if (instruct->directive != NULL && strcmp(instruct->directive , "START") == 0)	//opcode == start
	{
		arg = instruct->operand;
		len = parsing_argument(&arg, &parse);
		if (len == 0)
		{
			asm_error(ERROR_LACK_ARG, line);
			return 0;	//not start addr error
		}
		
		start_addr = hex_to_int(parse);	

		if (start_addr < 0)				
		{
			asm_error(start_addr, line);
			return 0;
		}
		if (instruct->label)
			strcpy(program_name, instruct->label);
		locctr = start_addr;
		instruct->loc = start_addr;

		if (read_line(asm_fp, buffer, tmp_buffer, &line) == 0)
			asm_error(ERROR_EMPTY_ASM, line);
		instruct = make_instruction(buffer, line);
		if (instruct == NULL)
			return 0;
	}

	while(1)
	{
		if (instruct->directive != NULL && strcmp(instruct->directive, "END") == 0)	//normal exit
			break;

		if (instruct->comment == NULL)	//not comment
		{
			int tmp = locctr;
			if (instruct->label != NULL)		//there is a symbol in the label field
			{
				if (find_symbol(instruct->label) != NULL)		//symbol duplicate
				{
					asm_error(ERROR_DUP_SYM, line);
					return 0;
				}
				add_symbol(instruct->label, locctr);
			}

			if (instruct->mnemonic != NULL)		//mnemonic
			{
				instruct->loc = tmp;
				int extend_flag = 0;
				onptr op;
				if (instruct->mnemonic[0] == '+')
				{
					extend_flag = 1;
					op = find_mnemonic(instruct->mnemonic + 1);
				}
				else
					op = find_mnemonic(instruct->mnemonic);
				
				
				if (op->format[0] == '1')
					locctr += 1;
				else if (op->format[0] == '2')
					locctr += 2;
				else if (op->format[0] == '3')
					locctr += 3 + extend_flag;
				else
				{
					asm_error(ERROR_WRONG_FORMAT, line);
					return 0;
				}
			}
			else if (instruct->directive != NULL)	//directive
			{
				if (strcmp(instruct->directive, "WORD") == 0)
				{
					instruct->loc = tmp;
					locctr += 3;
				}
				if (strcmp(instruct->directive, "RESW") == 0)	
				{												
					arg = instruct->operand;
					len = parsing_argument(&arg, &parse);

					if (len == 0)
					{
						asm_error(ERROR_LACK_ARG,line);
						return 0;
					}
					int num = dec_to_int(parse);
					if (num < 0)
					{
						asm_error(num, line);
						return 0;
					}
					instruct->loc = tmp;
					locctr += num * 3;
				}
				if (strcmp(instruct->directive, "RESB") == 0)
				{
					arg = instruct->operand;
					len = parsing_argument(&arg, &parse);

					if (len == 0)
					{
						asm_error(ERROR_LACK_ARG, line);
						return 0;
					}
					int num = dec_to_int(parse);
					if (num < 0)
					{
						asm_error(num, line);
						return 0;
					}
					instruct->loc = tmp;
					locctr += num;
				}
				if (strcmp(instruct->directive, "BYTE") == 0)
				{
					arg = instruct->operand;
					len = parsing_argument(&arg, &parse);

					if (len <= 3)
					{
						asm_error(ERROR_LACK_ARG, line);
						return 0;
					}

					instruct->loc = tmp;
					if (parse[0] == 'C' && parse[1] == '\'' && parse[len - 1] == '\'')
						locctr += len - 3;
					else if (parse[0] == 'X' && parse[1] == '\'' && parse[len - 1] == '\'')
						locctr += (len - 2) / 2;
					else
					{
						asm_error(ERROR_WRONG_FORMAT, line);
						return 0;
					}
				}
			}
		}
		if (read_line(asm_fp, buffer, tmp_buffer, &line) == 0)
			asm_error(ERROR_EMPTY_ASM, line);
		instruct = make_instruction(buffer, line);
		if (instruct == NULL)
			return 0;
	}


	if (instruct->directive != NULL && strcmp(instruct->directive, "END") != 0)
	{
		asm_error(ERROR_NOT_END, line);
		return 0;
	}
	program_length = locctr - start_addr;
	return 1;
}

//assembler pass2 process
//write list file and obj file
int assemble_pass2(FILE* list_fp, FILE* obj_fp)
{
	inptr cur = intermediate;
	char *arg = cur->operand;
	char *parse;
	int len;
	int record_start = 0;
	int start_addr = 0;
	object_code obj;
	char obj_str[OBJCODE_SIZE];
	char record[RECODE_SIZE];

	if (cur->directive && strcmp(cur->directive, "START") == 0)
	{
		arg = cur->operand;
		len = parsing_argument(&arg, &parse);
		start_addr = hex_to_int(parse);

		len = parsing_argument(&arg, &parse);

		if (len)
		{
			asm_error(ERROR_OVER_ARG, cur->line);
			return 0;
		}

		write_list_line(list_fp, cur, obj_str);
		cur = cur->link;
	}
	write_obj_line(obj_fp,'H', start_addr, NULL);
	record[0] = '\0';
	record_start = start_addr;

	while (cur)
	{
		if (cur->directive && strcmp(cur->directive, "END") == 0)	//normal exit
			break;

		if (cur->comment == NULL)
		{
			if (cur->directive != NULL && strcmp(cur->directive, "BASE") == 0)
			{
				base_flag = 1;
				arg = cur->operand;
				len = parsing_argument(&arg, &parse);
				if (len == 0)
				{
					asm_error(ERROR_LACK_ARG, cur->line);
					return 0;
				}
				snptr symbol = find_symbol(parse);
				if (symbol == NULL)
				{
					asm_error(ERROR_NOT_SYM, cur->line);
					return 0;
				}
				base = symbol->loc;

				len = parsing_argument(&arg, &parse);
				if (len)
				{
					asm_error(ERROR_OVER_ARG, cur->line);
					return 0;
				}
			}
			else if (cur->directive != NULL && strcmp(cur->directive, "NOBASE") == 0)
			{
				base_flag = 0;
				arg = cur->operand;
				len = parsing_argument(&arg, &parse);
				if (len)
				{
					asm_error(ERROR_OVER_ARG, cur->line);
					return 0;
				}
			}
			else
			{
				if (make_obj(cur, &obj) == 0)
					return 0;

				len = obj_to_str(obj, obj_str);	

				if (len < 0)
				{
					asm_error(len, cur->line);
					return 0;
				}

				if (record_start + 0x1D <= cur->loc)	//new line
				{
					write_obj_line(obj_fp, 'T', record_start, record);
					record[0] = '\0';
					record_start = cur->loc;
				}
				strcat(record, obj_str);	//add object code in text record
			}
		}
		write_list_line(list_fp, cur, obj_str);
		cur = cur->link;
	}

	write_obj_line(obj_fp,'T', record_start, record);
	write_obj_line(obj_fp,'M',0,NULL);

	arg = cur->operand;
	if (arg && (len = parsing_argument(&arg, &parse)))
	{
		snptr symbol = find_symbol(parse);
		if (symbol == NULL)
		{
			asm_error(ERROR_NOT_SYM, cur->line);
			return 0;
		}
		start_addr = symbol->loc;
		len = parsing_argument(&arg, &parse);
		if (len)
		{
			asm_error(ERROR_OVER_ARG, cur->line);
			return 0;
		}
	}
	write_obj_line(obj_fp,'E', start_addr , NULL);
	write_list_line(list_fp, cur, obj_str);
	return 1;
}

//read line from file pointer fp
//and refine this line and store in buffer
//calculate line
int read_line(FILE* fp, char buffer[], char tmp_buffer[], int *line)
{
	do
	{
		if (fgets(tmp_buffer, FILE_BUFFER_SIZE, fp) == NULL)
			return 0;
	} while (tmp_buffer[0] == '\n');

	*line += 5;
	refine_argument(tmp_buffer, buffer);
	
	return 1;
}

//make instruction from argument
//categorize instruction (ex. comment directive, mnemonic)
//create intermediate file
inptr make_instruction(char arg[], int line)
{
	inptr pnew = (inptr)malloc(sizeof(*pnew));
	pnew->comment = pnew->label = pnew->directive = pnew->mnemonic = pnew->operand = NULL;
	pnew->link = NULL;
	pnew->loc = 0;
	pnew->line = line;

	char *parse;
	int len = parsing_argument(&arg, &parse);
	
	if (parse[0] == '.')		//comment
	{
		pnew->comment = (char*)malloc(sizeof(char)*INSTRUCTION_BUFFER);
		int start = 0;
		strcpy((pnew->comment) + start, parse + 1);
		start = len - 1;
		while (len = parsing_argument(&arg, &parse))
		{
			strcpy((pnew->comment) + start, parse);
			start += len;
		}
		pnew->comment[strlen(pnew->comment) + 1] = '\0';
		add_instruction(pnew);

		return pnew;
	}
	
	if (is_mnemonic(parse) == 0 && is_directive(parse) == 0)		//label
	{
		pnew->label = (char*)malloc(sizeof(char)*INSTRUCTION_BUFFER);
		strcpy(pnew->label, parse);
		len = parsing_argument(&arg, &parse);
		pnew->label[strlen(pnew->label) + 1] = '\0';
	}

	if (is_mnemonic(parse))	//mnemonic
	{
		pnew->mnemonic = (char*)malloc(sizeof(char)*INSTRUCTION_BUFFER);
		strcpy(pnew->mnemonic, parse);
		pnew->mnemonic[strlen(pnew->mnemonic) + 1] = '\0';
	}
	else if (is_directive(parse))//directive
	{
		pnew->directive = (char*)malloc(sizeof(char)*INSTRUCTION_BUFFER);
		strcpy(pnew->directive, parse);
		pnew->directive[strlen(pnew->directive) + 1] = '\0';
	}
	else
	{	
		asm_error(ERROR_WRONG_FORMAT, line);
		return NULL;
	}
	pnew->operand = (char*)malloc(sizeof(char)*INSTRUCTION_BUFFER);
	pnew->operand[0] = '\0';
	int start = 0;

	while (len = parsing_argument(&arg, &parse))
	{
		strcpy((pnew->operand) + start, parse);
		start += len+1;
	}

	*((pnew->operand) + start) = '\0';
	add_instruction(pnew);

	return pnew;

	
	
}

//clear intermediate file
void clear_intermediate()
{
	inptr p = intermediate;

	while (p)
	{
		inptr next = p->link;
		if (p->comment)
			free(p->comment);
		if (p->label)
			free(p->label);
		if (p->mnemonic)
			free(p->mnemonic);
		if (p->operand)
			free(p->operand);
		free(p);
		p = next;
	}

	intermediate = NULL;
	rear = NULL;
}

//add instruction to intermediate file
void add_instruction(inptr pnew)
{
	if (intermediate == NULL)
	{
		intermediate = pnew;
		rear = pnew;
		return;
	}

	rear->link = pnew;
	rear = pnew;
}

//return 1 if string == mnemonic else return 0
int is_mnemonic(char mnemonic[])
{
	if (find_mnemonic(mnemonic) != NULL)
		return 1;

	if (mnemonic[0] == '+' && find_mnemonic(mnemonic + 1) != NULL)
		return 1;

	return 0;
}

//return 1 if string == directive else return 0
int is_directive(char directive[])
{
	if (strcmp(directive, "START") == 0)
		return 1;
	if (strcmp(directive, "END") == 0)
		return 1;
	if (strcmp(directive, "BASE") == 0)
		return 1;
	if (strcmp(directive, "NOBASE") == 0)
		return 1;
	if (strcmp(directive, "BYTE") == 0)
		return 1;
	if (strcmp(directive, "WORD") == 0)
		return 1;
	if (strcmp(directive, "RESB") == 0)
		return 1;
	if (strcmp(directive, "RESW") == 0)
		return 1;

	return 0;
}

//return 1 if string == register else return 0 
int is_register(char reg[])
{
	if (strcmp(reg, "A") == 0)
		return 1;
	if (strcmp(reg, "X") == 0)
		return 1;
	if (strcmp(reg, "L") == 0)
		return 1;
	if (strcmp(reg, "B") == 0)
		return 1;
	if (strcmp(reg, "S") == 0)
		return 1;
	if (strcmp(reg, "T") == 0)
		return 1;
	if (strcmp(reg, "F") == 0)
		return 1;
	if (strcmp(reg, "PC") == 0)
		return 1;
	if (strcmp(reg, "SW") == 0)
		return 1;
	return 0;
}

//add symbol to symbol table
void add_symbol(char symbol[], int loc)
{
	int hash_number = hash_function(symbol);
	snptr pnew = (snptr)malloc(sizeof(*pnew));
	strcpy(pnew->symbol, symbol);
	pnew->loc = loc;
	add_hash(sym_table, pnew, hash_number);
	sym_table_num++;
}

//find symbol in symbol table if not found return NULL
snptr find_symbol(char symbol[])
{
	int hash_number = hash_function(symbol);

	for (hnptr p = sym_table[hash_number]; p; p = p->link)
	{
		snptr sp = p->data;
		if (strcmp(sp->symbol, symbol) == 0)
			return sp;
	}

	return NULL;
}

//make object code from intermediate node
//return 1 if success else return 0
int make_obj(inptr cur, object_code *obj)
{
	int extend_flag = 0;
	int indexed_flag = 1;
	int immediate_flag = 1;
	int x_flag = 0;

	char *arg = cur->operand;
	char *parse;
	int len;
	int disp;

	obj->format = 0;	//not object code
	obj->code = 0;

	if (cur->mnemonic != NULL)
	{
		char *mnemonic = cur->mnemonic;
		if (mnemonic[0] == '+')
		{
			extend_flag = 1;
			mnemonic++;
		}
		onptr op = find_mnemonic(mnemonic);
		snptr symbol = NULL;
		int constant = 0;

		if (op->format[0] != '3' && extend_flag == 1)		// extend format
		{
			asm_error(ERROR_WRONG_FORMAT, cur->line);
			return 0;
		}


		if (op->format[0] == '3')	//format 3 or 4
		{
			if (extend_flag)
				obj->format = 4;
			else
				obj->format = 3;

			len = parsing_argument(&arg, &parse);

			if (strcmp(op->mnemonic, "RSUB") == 0)
			{
				if (len)
				{
					asm_error(ERROR_OVER_ARG, cur->line);
					return 0;
				}
				obj->code = op->opcode + 3;	//simple addressing mode
				if (extend_flag)
					obj->code *= 1 << 24;
				else
					obj->code *= 1 << 16;
				return 1;

			}

			if (len == 0)
			{
				asm_error(ERROR_LACK_ARG, cur->line);
				return 0;
			}
			if (parse[0] == '@')
			{
				parse++;
				indexed_flag = 1;
				immediate_flag = 0;
			}
			else if (parse[0] == '#')
			{
				parse++;
				indexed_flag = 0;
				immediate_flag = 1;
			}

			snptr symbol = find_symbol(parse);
			int constant = 0;
			if (symbol == NULL)		//not symbol
			{
				constant = dec_to_int(parse);
				if (constant < 0)		//not symbol not constant
				{
					asm_error(constant, cur->line);
					return 0;
				}
			}

			len = parsing_argument(&arg, &parse);

			if (len && indexed_flag == 1 && immediate_flag == 1 && parse[0] == ',')	//x flag only simple
			{
				len = parsing_argument(&arg, &parse);
				if (len && strcmp(parse, "X") == 0)
					x_flag = 1;
				else
				{
					asm_error(ERROR_WRONG_FORMAT, cur->line);
					return 0;
				}

				len = parsing_argument(&arg, &parse);
				if (len)
				{
					asm_error(ERROR_OVER_ARG, cur->line);
					return 0;
				}
			}
			else if (len)
			{
				asm_error(ERROR_OVER_ARG, cur->line);
				return 0;
			}


			obj->code = (op->opcode) >> 2;
			obj->code = obj->code * 2 + indexed_flag;
			obj->code = obj->code * 2 + immediate_flag;
			obj->code = obj->code * 2 + x_flag;

			
			if (extend_flag)
			{
				obj->code = obj->code * 8 + extend_flag;
				obj->code *= 1 << 20;

				if (immediate_flag == 1 && indexed_flag == 0)
				{
					if (symbol == NULL)
						disp = constant;
					else
						disp = symbol->loc;
				}
				if (indexed_flag == 1)	//indexed addressing mode + simple addressing_mode
				{
					if (symbol == NULL)
					{
						asm_error(ERROR_NOT_SYM, cur->line);
						return 0;
					}
					disp = symbol->loc;
					modification_table[mdf_num++] = cur->loc;
				}
				obj->code += disp;
			}
			else			//format 3
			{
				obj->code *= 1 << 15;

				if (immediate_flag == 1 && indexed_flag == 0)
				{
					if (symbol == NULL)
						disp = constant;
					else
						disp = symbol->loc;
				}
				if (indexed_flag == 1)	//simple, indexed
				{
					if (symbol == NULL)
					{
						asm_error(ERROR_NOT_SYM, cur->line);
						return 0;
					}
					disp = symbol->loc;
				}

				if (symbol != NULL)
				{
					obj->code += (1 << 13);
					disp -= (cur->loc + 3);
					if (disp < -2048 || disp > 2047)		//base relative 
					{
						obj->code += (1 << 14) - (1 << 13);	//base relative?
						disp = symbol->loc - base;
						if (disp < 0 || disp > 4095 || base_flag == 0)
						{
							asm_error(ERROR_ADDR_MODE, cur->line);		
							return 0;
						}
					}
					else
					{
						disp = disp & 0x00000fff;
					}
				}
				if (extend_flag)
				{
					if (disp >= 1 << 20)
					{
						asm_error(ERROR_VALUE_RANGE, cur->line);
						return 0;
					}
				}
				else
				{
					if (disp >= 1 << 12)
					{
						asm_error(ERROR_VALUE_RANGE, cur->line);
						return 0;
					}
				}
				obj->code += disp;
			}
		}
		else if (op->format[0] == '2')
		{
			snptr symbol = NULL;
			int r1 = FLAG_EMPTY;
			int r2 = FLAG_EMPTY;
			arg = cur->operand;
			obj->format = 2;
			len = parsing_argument(&arg, &parse);
			if (len == 0)
			{
				asm_error(ERROR_LACK_ARG, cur->line);
				return 0;
			}
			
			symbol = find_symbol(parse);
			if (symbol == NULL)
			{
				r1 = hex_to_int(parse);
				if (r1 < 0)
				{
					asm_error(r1, cur->line);
					return 0;
				}
				if (r1 >= 16)
				{
					asm_error(ERROR_VALUE_RANGE, cur->line);
					return 0;
				}
			}
			else
				r1 = symbol->loc;

			len = parsing_argument(&arg, &parse);

			if (len)
			{
				if (parse[0] == ',')
				{
					len = parsing_argument(&arg, &parse);
					symbol = find_symbol(parse);
					if (symbol == NULL)
					{
						r2 = hex_to_int(parse);
						if (r2 < 0)	
						{
							asm_error(r2, cur->line);
							return 0;
						}
						if (r2 == 0 || r2 > 16)
						{
							asm_error(ERROR_VALUE_RANGE, cur->line);
							return 0;
						}
						r2--;	//r2 = n-1;
					}
					else
						r2 = symbol->loc;
					len = parsing_argument(&arg, &parse);
					
					if (len)
					{
						asm_error(ERROR_OVER_ARG, cur->line);
						return 0;
					}
				}
				else
				{
					asm_error(ERROR_WRONG_FORMAT, cur->line);
					return 0;
				}
			}
			
			if (strcmp(op->mnemonic, "CLEAR") == 0)
			{
				if (r1 == FLAG_EMPTY || r2 != FLAG_EMPTY)
				{
					asm_error(ERROR_WRONG_FORMAT, cur->line);
					return 0;
				}
			}
			else if (strcmp(op->mnemonic, "SVC") == 0 )
			{
				if (r1 == FLAG_EMPTY || r2 != FLAG_EMPTY)
				{
					asm_error(ERROR_WRONG_FORMAT, cur->line);
					return 0;
				}
			}
			else if (strcmp(op->mnemonic, "TIXR") == 0)
			{
				if(r1 == FLAG_EMPTY || r2 != FLAG_EMPTY)
				{
					asm_error(ERROR_WRONG_FORMAT, cur->line);
					return 0;
				}
			}
			else if ((r1 == FLAG_EMPTY || r2 == FLAG_EMPTY))
			{
				if (r1 == FLAG_EMPTY || r2 != FLAG_EMPTY)
				{
					asm_error(ERROR_WRONG_FORMAT, cur->line);
					return 0;
				}
			}
			if (r2 == FLAG_EMPTY)
				r2 = 0;
			obj->code = (op->opcode);
			obj->code = obj->code << 4;
			obj->code += r1;
			obj->code = obj->code << 4;
			obj->code += r2;
		}
		else if (op->format[0] == '1')
		{
			obj->format = 1;
			len = parsing_argument(&arg, &parse);
			if (len)
			{
				asm_error(ERROR_OVER_ARG, cur->line);
				return 0;
			}
			obj->code += op->opcode;
		}
	}
	else if (cur->directive != NULL)
	{
		obj->code = 0;
		if (strcmp(cur->directive, "BYTE") == 0)
		{
			len = parsing_argument(&arg, &parse);
			if (parse[0] == 'C')
			{
				for (int i = 2; i < len-1; i++)
				{
					obj->code *= 1 << 8;
					obj->code += parse[i];
				}
				obj->format = (len - 3) * 2;		
				obj->format = -obj->format;
			}
			else if (parse[0] == 'X')
			{
				for (int i = 2; i < len - 1; i++)
				{
					obj->code *= 1 << 4;
					if ('0' <= parse[i] && parse[i] <= '9')
						obj->code += parse[i] - '0';
					else if ('A' <= parse[i] && parse[i] <= 'Z')
						obj->code += parse[i] - 'A' + 10;
					else
					{
						asm_error(ERROR_HEX_FORMAT, cur->line);
						return 0;
					}
				}
				obj->format = (len - 2) / 2 * 2;
				obj->format = -obj->format;
			}

			len = parsing_argument(&arg, &parse);
			if (len)
			{
				asm_error(ERROR_OVER_ARG, cur->line);
				return 0;
			}
		}
		else if (strcmp(cur->directive, "WORD") == 0)
		{
			len = parsing_argument(&arg, &parse);
			int num = dec_to_int(parse);
			if (num < 0)
			{
				asm_error(num, cur->line);
				return 0;
			}
			if (num > 0xFFFFFF)	
			{
				asm_error(ERROR_VALUE_RANGE, cur->line);
				return 0;
			}
			obj->code = num;
		}
	}
	return 1;
}

//transform obj code to string
//if sucess return 1 else return 0
int obj_to_str(object_code obj, char str[])
{
	int len = 0;
	int error = 0;

	if (obj.format == 0)
	{
		str[0] = '\0';
		return 0;
	}
	if (obj.format == 1)
	{
		error = int_to_hex(obj.code, str, 2);
		if (error < 0)
		{
			return error;
		}
		return 2;
	}
	if (obj.format == 2)
	{
		error = int_to_hex(obj.code, str, 4);
		if (error < 0)
		{
			return error;
		}
		return 4;
	}
	if (obj.format == 3)
	{
		error = int_to_hex(obj.code, str, 6);
		if (error < 0)
		{
			return error;
		}
		return 6;
	}
	if (obj.format == 4)
	{
		error = int_to_hex(obj.code, str, 8);
		if (error < 0)
		{
			return error;
		}
		return 8;
	}
	if (obj.format < 0)
	{
		error = int_to_hex(obj.code, str, -obj.format);
		if (error < 0)
		{
			return error;
		}
		return -obj.format;
	}

	return 0;
}

//write 1 line in obj file
void write_obj_line(FILE *fp, char type, int record_start, char record[])
{
	if (type == 'H')
	{
		fprintf(fp, "H%-6s",program_name);
		fprintf(fp, "%06X", record_start);
		fprintf(fp, "%06X\n", program_length);
	}
	if (type == 'T')
	{
		int len = strlen(record);
		fprintf(fp, "T%06X", record_start);
		fprintf(fp, "%02X", len/2);
		fprintf(fp, "%s\n", record);
	}
	if (type == 'E')
	{
		fprintf(fp, "E%06X\n", record_start);
	}
	if(type == 'M')
	{
		for(int i = 0 ;i<mdf_num;i++)
		{
			//support only extend format modification
			fprintf(fp,"M%06X05+01\n",modification_table[i] + 1);
		}
	}
}

//write 1 line to list file
void write_list_line(FILE *fp, inptr cur, char obj[])
{
	fprintf(fp, "%-5d\t", cur->line);

	if (cur->comment)
	{
		fprintf(fp, ". %s\n", cur->comment);
		return;
	}
	if (cur->directive != NULL && (strcmp(cur->directive, "BASE") == 0 || strcmp(cur->directive, "NOBASE") == 0 || strcmp(cur->directive, "END") == 0))
		fprintf(fp, "%4s\t", " ");
	else
		fprintf(fp, "%04X\t", cur->loc);

	if (cur->label)
		fprintf(fp, "%-10s", cur->label);
	else
		fprintf(fp, "%-10s","");

	if (cur->directive)
		fprintf(fp, "%-10s\t", cur->directive);
	if (cur->mnemonic)
		fprintf(fp, "%-10s\t", cur->mnemonic);

	if (cur->operand)
	{
		char *arg = cur->operand;
		char *parse;
		int len;
		while (len = parsing_argument(&arg, &parse))
		{
			parse[len] = ' ';
		}
		fprintf(fp, "%-20s", cur->operand);
	}
	if (cur->directive != NULL && (strcmp(cur->directive, "BASE") == 0 || strcmp(cur->directive, "NOBASE") == 0 || strcmp(cur->directive, "START") == 0 || strcmp(cur->directive, "END") == 0))
		;
	else
		fprintf(fp, "%s", obj);
	fprintf(fp,"\n");
}

//get sym_table address
hnptr* get_sym_table_addr()
{
	return sym_table;
}
