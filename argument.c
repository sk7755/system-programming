#include "argument.h"

int symbol_flag = 0;

//if argument have help command and set right format, execute help command and add history
//else print error message
//return 1 if argument have help command (even if wrong format) else return 0
int is_help(char arg[], char tmp_arg[])
{
	char *parse;
	int len = 0;

	len = parsing_argument(&arg, &parse);

	if (strcmp(parse, "help") == 0 || strcmp(parse, "h") == 0)
	{
		len = parsing_argument(&arg, &parse);
		if (len)
		{
			error_print(ERROR_OVER_ARG);
			return 1;
		}
		cmd_help();
		add_history(tmp_arg);
		return 1;
	}

	return 0;
}

//if argument have dir command and set right format, execute dir command and add history
//else print error message
//return 1 if argument have dir command (even if wrong format) else return 0 
int is_dir(char arg[], char tmp_arg[])
{
	char *parse;
	int len = 0;

	len = parsing_argument(&arg, &parse);

	if (strcmp(parse, "dir") == 0 || strcmp(parse, "d") == 0)
	{
		len = parsing_argument(&arg, &parse);
		if (len)
		{
			error_print(ERROR_OVER_ARG);
			return 1;
		}

		int error = cmd_dir();
		if (error == ERROR_DIR_OPEN || error == ERROR_DIR_CLOSE)
		{
			error_print(error);
			return 1;
		}
		add_history(tmp_arg);
		return 1;
	}

	return 0;
}


int is_type(char arg[], char tmp_arg[])
{
	char *parse;
	int len = 0;

	len = parsing_argument(&arg, &parse);

	if (strcmp(parse, "type") == 0)
	{
		len = parsing_argument(&arg, &parse);

		if (len == 0)
		{
			error_print(ERROR_LACK_ARG);
			return 1;
		}

		char *filename = parse;

		len = parsing_argument(&arg, &parse);

		if (len)
		{
			error_print(ERROR_OVER_ARG);
			return 1;
		}

		int error = cmd_type(filename);

		if (error == ERROR_FILE_OPEN || error == ERROR_FILE_CLOSE)
		{
			error_print(error);
			return 1;
		}
		add_history(tmp_arg);
		return 1;
	}
	return 0;
}


//if argument have quit command and set right format, execute quit command
//else print error message
//return 1 if argument have quit command (even if wrong format) else return 0
int is_quit(char arg[])
{
	char *parse;
	int len = 0;

	len = parsing_argument(&arg, &parse);

	if (strcmp(parse, "quit") == 0 || strcmp(parse, "q") == 0)
	{
		len = parsing_argument(&arg, &parse);

		if (len)
		{
			error_print(ERROR_OVER_ARG);
			return 1;
		}

		cmd_quit();
		return 1;
	}

	return 0;
}

//if argument have history command and set right format, execute history command and add history
//else print error message
//return 1 if argument have history command (even if wrong format) else return 0
int is_history(char arg[], char tmp_arg[])
{
	char *parse;
	int len = 0;

	len = parsing_argument(&arg, &parse);

	if (strcmp(parse, "history") == 0 || strcmp(parse, "hi") == 0)
	{
		len = parsing_argument(&arg, &parse);

		if (len)
		{
			error_print(ERROR_OVER_ARG);
			return 1;
		}

		add_history(tmp_arg);
		cmd_history();
		return 1;
	}

	return 0;
}

//if argument have dump command and set right format, execute dump command and add history
//else print error message
//return 1 if argument have dump command (even if wrong format) else return 0
int is_dump(char arg[], char tmp_arg[])
{
	char *parse;
	int len = 0;

	len = parsing_argument(&arg, &parse);

	if (strcmp(parse, "dump") == 0 || strcmp(parse, "du") == 0)
	{
		int start = FLAG_EMPTY, end = FLAG_EMPTY;
		
		len = parsing_argument(&arg, &parse);

		if (len)
		{
			start = hex_to_int(parse);

			if (start == ERROR_HEX_FORMAT)
			{
				error_print(ERROR_HEX_FORMAT);
				return 1;
			}

			if (start == ERROR_HEX_RANGE)
			{
				error_print(ERROR_HEX_RANGE);
				return 1;
			}

			len = parsing_argument(&arg, &parse);

			if (len)
			{
				if (strcmp(parse, ",") != 0)
				{
					error_print(ERROR_LACK_ARG);
					return 1;
				}

				len = parsing_argument(&arg, &parse);

				if (len == 0)
				{
					error_print(ERROR_LACK_ARG);
					return 1;
				}

				end = hex_to_int(parse);

				if (end == ERROR_HEX_FORMAT)
				{
					error_print(ERROR_HEX_FORMAT);
					return 1;
				}

				if (end == ERROR_HEX_RANGE)
				{
					error_print(ERROR_HEX_RANGE);
					return 1;
				}

				len = parsing_argument(&arg, &parse);

				if (len)
				{
					error_print(ERROR_OVER_ARG);
					return 1;
				}
			}
		}
		int error = cmd_dump(start, end);
		if (error == ERROR_SE_RANGE)
		{
			error_print(ERROR_SE_RANGE);
			return 1;
		}
		else if (error == ERROR_MEM_BOUND)
		{
			error_print(ERROR_MEM_BOUND);
			return 1;
		}
		add_history(tmp_arg);
		return 1;
	}

	return 0;
}

//if argument have edit command and set right format, execute edit command and add history
//else print error message
//return 1 if argument have edit command (even if wrong format) else return 0
int is_edit(char arg[], char tmp_arg[])
{
	char *parse;
	int len = 0;

	len = parsing_argument(&arg, &parse);

	if (strcmp(parse, "edit") == 0 || strcmp(parse, "e") == 0)
	{
		len = parsing_argument(&arg, &parse);

		if (len == 0)
		{
			error_print(ERROR_LACK_ARG);
			return 1;
		}

		int addr = hex_to_int(parse);

		if (addr == ERROR_HEX_FORMAT)
		{
			error_print(ERROR_HEX_FORMAT);
			return 1;
		}

		if (addr == ERROR_HEX_RANGE)
		{
			error_print(ERROR_HEX_RANGE);
			return 1;
		}

		len = parsing_argument(&arg, &parse);

		if (len == 0 || strcmp(parse, ",") != 0)
		{
			error_print(ERROR_LACK_ARG);
			return 1;
		}

		len = parsing_argument(&arg, &parse);

		if (len == 0)
		{
			error_print(ERROR_LACK_ARG);
			return 1;
		}

		int value = hex_to_int(parse);

		if (value == ERROR_HEX_FORMAT)
		{
			error_print(ERROR_HEX_FORMAT);
			return 1;
		}

		if (value == ERROR_HEX_RANGE)
		{
			error_print(ERROR_HEX_RANGE);
			return 1;
		}

		len = parsing_argument(&arg, &parse);

		if (len)
		{
			error_print(ERROR_OVER_ARG);
			return 1;
		}

		int error = cmd_edit(addr, value);

		if (error == ERROR_VALUE_RANGE)
		{
			error_print(ERROR_VALUE_RANGE);
			return 1;
		}
		add_history(tmp_arg);
		return 1;
	}

	return 0;
}

//if argument have fill command and set right format, execute fill command and add history
//else print error message
//return 1 if argument have fill command (even if wrong format) else return 0
int is_fill(char arg[], char tmp_arg[])
{
	char *parse;
	int len = 0;

	len = parsing_argument(&arg, &parse);

	if (strcmp(parse, "fill") == 0 || strcmp(parse, "f") == 0)
	{
		len = parsing_argument(&arg, &parse);

		if (len == 0)
		{
			error_print(ERROR_LACK_ARG);
			return 1;
		}

		int start = hex_to_int(parse);

		if (start == ERROR_HEX_FORMAT || start == ERROR_HEX_RANGE)
		{
			error_print(start);
			return 1;
		}

		len = parsing_argument(&arg, &parse);

		if (len == 0 || strcmp(parse, ",") != 0)
		{
			error_print(ERROR_LACK_ARG);
			return 1;
		}

		len = parsing_argument(&arg, &parse);

		if (len == 0)
		{
			error_print(ERROR_LACK_ARG);
			return 1;
		}

		int end = hex_to_int(parse);

		if (end == ERROR_HEX_FORMAT || end == ERROR_HEX_RANGE)
		{
			error_print(end);
			return 1;
		}

		len = parsing_argument(&arg, &parse);

		if (len == 0 || strcmp(parse, ",") != 0)
		{
			error_print(ERROR_LACK_ARG);
			return 1;
		}

		len = parsing_argument(&arg, &parse);

		if (len == 0)
		{
			error_print(ERROR_LACK_ARG);
			return 1;
		}

		int value = hex_to_int(parse);

		if (value == ERROR_HEX_FORMAT || value == ERROR_HEX_RANGE)
		{
			error_print(value);
			return 1;
		}

		len = parsing_argument(&arg, &parse);

		if (len)
		{
			error_print(ERROR_OVER_ARG);
			return 1;
		}

		int error = cmd_fill(start, end, value);
		if (error == ERROR_VALUE_RANGE || error == ERROR_SE_RANGE)
		{
			error_print(error);
			return 1;
		}

		add_history(tmp_arg);
		return 1;
	}

	return 0;
}

//if argument have reset command and set right format, execute reset command and add history
//else print error message
//return 1 if argument have reset command (even if wrong format) else return 0
int is_reset(char arg[], char tmp_arg[])
{
	char *parse;
	int len = 0;

	len = parsing_argument(&arg, &parse);

	if (strcmp(parse, "reset") == 0)
	{
		len = parsing_argument(&arg, &parse);

		if (len)
		{
			error_print(ERROR_OVER_ARG);
			return 1;
		}

		cmd_reset();
		add_history(tmp_arg);
		return 1;
	}

	return 0;
}

//if argument have opcodelist command and set right format, execute opcodelist command and add history
//else print error message
//return 1 if argument have opcodelist command (even if wrong format) else return 0
int is_opcodelist(char arg[], char tmp_arg[])
{
	char *parse;
	int len = 0;

	len = parsing_argument(&arg, &parse);

	if (strcmp(parse, "opcodelist") == 0)
	{
		len = parsing_argument(&arg, &parse);

		if (len)
		{
			error_print(ERROR_OVER_ARG);
			return 1;
		}

		cmd_opcodelist();
		add_history(tmp_arg);
		return 1;
	}
	return 0;
}

//if argument have opcode mnemonic command and set right format, execute opcode mnemonic command and add history
//else print error message
//return 1 if argument have opcode mnemonic command (even if wrong format) else return 0
int is_opcode_mnemonic(char arg[], char tmp_arg[])
{
	char *parse;
	int len = 0;

	len = parsing_argument(&arg, &parse);

	if (strcmp(parse, "opcode") == 0)
	{
		len = parsing_argument(&arg, &parse);

		if (len == 0)
		{
			error_print(ERROR_LACK_ARG);
			return 1;
		}

		char* mnemonic = parse;
		
		len = parsing_argument(&arg, &parse);

		if (len)
		{
			error_print(ERROR_OVER_ARG);
			return 1;
		}

		int error = cmd_opcode_mnemonic(mnemonic);

		if (error == ERROR_NOT_MNEMONIC)
		{
			error_print(error);
			return 1;
		}
		
		add_history(tmp_arg);

		return 1;
	}
	return 0;
}

//check if argument is assemble command
//if this is assemble command return 1 even if error exist else return 0
int is_assemble(char arg[], char tmp_arg[])
{
	char *parse;
	int len = 0;

	len = parsing_argument(&arg, &parse);

	if (strcmp(parse, "assemble") == 0)
	{
		symbol_flag = 0;
		len = parsing_argument(&arg, &parse);

		if (len == 0)
		{
			error_print(ERROR_LACK_ARG);
			return 1;
		}

		char *filename = parse;

		len = parsing_argument(&arg, &parse);

		if (len)
		{
			error_print(ERROR_OVER_ARG);
			return 1;
		}
		if (cmd_assemble(filename))
		{
			symbol_flag = 1;
			add_history(tmp_arg);
		}
		return 1;
	}
	return 0;
}


//check if argument is symbol command
//if this is symbol command return 1 even if error exist else return 0
int is_symbol(char arg[], char tmp_arg[])
{
	char *parse;
	int len = 0;

	len = parsing_argument(&arg, &parse);

	if (strcmp(parse, "symbol") == 0)
	{
		len = parsing_argument(&arg, &parse);
		if (len)
		{
			error_print(ERROR_OVER_ARG);
			return 1;
		}
		if(symbol_flag)
			cmd_symbol();
		else
			printf("there is not recent success asm file\n");
		add_history(tmp_arg);
		
		return 1;
	}
	return 0;
}

int is_loader(char arg[], char tmp_arg[])
{
	char *parse;
	int len = 0;

	len = parsing_argument(&arg, &parse);

	if (strcmp(parse, "loader") == 0)
	{
		int cnt = 0;
		char filename[_MAX_LINK][INPUT_SIZE];
		while (len = parsing_argument(&arg, &parse))
		{
			if (cnt >= _MAX_LINK)
			{
				error_print(ERROR_LINK_CNT);
				return 1;
			}
			strcpy(filename[cnt++], parse);
		}
		
		if (cmd_loader(filename,cnt))
		{
			add_history(tmp_arg);
		}
		return 1;
	}

	return 0;
}

int is_progaddr(char arg[], char tmp_arg[])
{
	char *parse;
	int len = 0;
	len = parsing_argument(&arg, &parse);

	if (strcmp(parse, "progaddr") == 0)
	{
		len = parsing_argument(&arg, &parse);

		if(len == 0)
		{
			error_print(ERROR_LACK_ARG);
			return 1;
		}
		int value = hex_to_int(parse);

		if (value == ERROR_HEX_FORMAT || value == ERROR_HEX_RANGE)
		{
			error_print(value);
			return 1;
		}

		len = parsing_argument(&arg, &parse);

		if (len)
		{
			error_print(ERROR_OVER_ARG);
			return 1;
		}

		cmd_progaddr(value);
		add_history(tmp_arg);
		return 1;
	}

	return 0;
}

int is_bp(char arg[], char tmp_arg[])
{
	char *parse;
	int len = 0;
	len = parsing_argument(&arg, &parse);

	if (strcmp(parse, "bp") == 0)
	{
		len = parsing_argument(&arg, &parse);

		if (len == 0)
		{
			//show all break point
			cmd_bp_show();
		}
		else if (strcmp(parse, "clear") == 0)
		{
			//clear all break point
			len = parsing_argument(&arg, &parse);
			if (len)
			{
				error_print(ERROR_OVER_ARG);
				return 1;
			}
			cmd_bp_clear();

		}
		else
		{
			//set break point
			int value = hex_to_int(parse);

			if (value == ERROR_HEX_FORMAT || value == ERROR_HEX_RANGE)
			{
				error_print(value);
				return 1;
			}

			len = parsing_argument(&arg, &parse);

			if (len)
			{
				error_print(ERROR_OVER_ARG);
				return 1;
			}
			cmd_bp_set(value);
		}

		add_history(tmp_arg);
		return 1;
	}
	return 0;
}

int is_run(char arg[], char tmp_arg[])
{
	char *parse;
	int len = 0;
	len = parsing_argument(&arg, &parse);

	if (strcmp(parse, "run") == 0)
	{
		len = parsing_argument(&arg, &parse);

		if (len)
		{
			error_print(ERROR_OVER_ARG);
			return 1;
		}

		cmd_run();
		add_history(tmp_arg);
		return 1;
	}
	return 0;
}

//refine argument to proper format (ex refined = "arg1'\0'arg2'\0'arg3\0\0")
//remove tab and space bar and divde comma from argument
void refine_argument(char arg[], char result[])
{
	int i = 0;
	while (arg[i] == ' ' || arg[i] == '\t')
		i++;

	char pre = 'a';
	int j = 0;
	for (; arg[i] != '\0'; i++)
	{
		if (arg[i] != ' ' && arg[i] != '\t')
		{
			if (arg[i] == ',' || pre == ',' || pre == ' ' || pre == '\t')
				result[j++] = '\0';
			result[j++] = arg[i];
		}
		pre = arg[i];
	}
	if (result[j - 1] == '\n')
		result[j - 1] = '\0';
	else
		result[j++] = '\0';
	result[j] = '\0';
}


//parsing construct from refined argument and set arg pointer to point next construct
//return length of construct and pointer of construct	
int parsing_argument(char **arg, char **parse)
{
	int len = strlen(*arg);

	*parse = *arg;
	*arg += len + 1;

	return len;
}

//change hex code to integer
//if hex code is not valid for given hex format return error number
//else return changed inter
int hex_to_int(char hex[])
{
	while (hex[0] == '0')
		hex++;

	int len = strlen(hex);

	if (len > 5)
		return ERROR_HEX_RANGE;	//overflow error

	int ret = 0;

	for (int i = 0; i < len; i++)
	{
		ret *= 16;
		if ('0' <= hex[i] && hex[i] <= '9')
			ret += hex[i] - '0';
		else if ('A' <= hex[i] && hex[i] <= 'F')
			ret += hex[i] - 'A' + 10;
		else if ('a' <= hex[i] && hex[i] <= 'f')
			ret += hex[i] - 'a' + 10;
		else
			return ERROR_HEX_FORMAT;	//It is not hex code
	}
	return ret;
}

//transform decimal string to integer
//if error exsit return error code else return 1
int dec_to_int(char dec[])
{
	while (dec[0] == '0')
		dec++;

	int len = strlen(dec);

	if (len > 9)
		return ERROR_DEC_RANGE;	//overflow error

	int ret = 0;

	for (int i = 0; i < len; i++)
	{
		ret *= 10;
		if ('0' <= dec[i] && dec[i] <= '9')
			ret += dec[i] - '0';
		else
			return ERROR_DEC_FORMAT;	//It is not dec code
	}

	return ret;
}

//transform integer to hexa code
//if error exist return error code else return 1
int int_to_hex(int num, char hex[], int length)
{
	int i = 0;
	while (num)
	{
		if (length - i - 1 < 0)
			return ERROR_VALUE_RANGE;

		int tmp = num % 16;
		if (tmp < 10)
			hex[length - i - 1] = tmp + '0';
		else
			hex[length - i - 1] = tmp - 10 + 'A';
		i++;
		num /= 16;
	}

	while (length - i - 1 >= 0)
	{
		hex[length - i - 1] = '0';
		i++;
	}
	hex[length] = '\0';

	return 1;
}

