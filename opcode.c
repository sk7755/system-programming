#include "opcode.h"

hnptr op_table[HASH_SIZE] = { NULL, };

//read opcode.txt and store in hash table
//if file open error or file close error return 0 and print error message
//else return 1
int read_opcode_file()
{
	FILE* fp = fopen("opcode.txt", "r");

	if (fp == NULL)
	{
		error_print(ERROR_FILE_OPEN);
		return 0;
	}

	char buffer[FILE_BUFFER_SIZE];
	char tmp_buffer[FILE_BUFFER_SIZE];

	while (fgets(buffer, FILE_BUFFER_SIZE, fp) != NULL)
	{
		strcpy(tmp_buffer, buffer);
		refine_argument(tmp_buffer, buffer);
		
		char *arg = buffer;

		char *parse;
		onptr pnew = (onptr)malloc(sizeof(*pnew));

		parsing_argument(&arg, &parse);
		pnew->opcode = hex_to_int(parse);
		parsing_argument(&arg, &parse);
		strcpy(pnew->mnemonic, parse);
		parsing_argument(&arg, &parse);
		strcpy(pnew->format, parse);

		int hash_number = hash_function(pnew->mnemonic);
		add_hash(op_table, pnew, hash_number);
	}

	if (fclose(fp) == EOF)
	{
		error_print(ERROR_FILE_CLOSE);
		return 0;
	}

	return 1;
}

//find precise mnemonic in hash table
//if success to find, return the mnemonic node else return null pointer 
onptr find_mnemonic(char mnemonic[])
{
	int hash_number = hash_function(mnemonic);
	
	for (hnptr p = op_table[hash_number]; p; p = p->link)
	{
		onptr op = p->data;
		if (strcmp(op->mnemonic, mnemonic) == 0)
			return op;
	}
		
	return NULL;
}

//execute opcodelist command
//show all opcode in each hash table number
void cmd_opcodelist()
{
	for (int i = 0; i < HASH_SIZE; i++)
	{
		printf("%d : ", i);
		for (hnptr p = op_table[i]; p; p = p->link)
		{
			onptr data = p->data;
			printf("[%s,%02X] ", data->mnemonic, data->opcode);
			if (p->link)
				printf("-> ");
		}
		printf("\n");
	}
}

//execute opcode mnemonic command
//show opcode of the mnemonic
int cmd_opcode_mnemonic(char mnemonic[])
{
	onptr op = find_mnemonic(mnemonic);
	if (op == NULL)
	{
		return ERROR_NOT_MNEMONIC;
	}
	printf("opcode is %02X\n", op->opcode);
	return 1;
}

//get op_table address
hnptr* get_op_table_addr()
{
	return op_table;
}
