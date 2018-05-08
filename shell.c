#include "shell.h"

typedef struct history_node
{
	char arg[INPUT_SIZE];
	hinptr link;
}history_node;

hinptr hn_head = NULL;
int history_cnt = 0;

//add argument to the linked-list 
void add_history(char arg[])
{
	history_cnt++;

	hinptr pnew = (hinptr)malloc(sizeof(*pnew));
	for (int i = 0; arg[i] != '\0';)
	{
		strcpy(pnew->arg, arg);
		i += strlen(arg + i) + 1;
	}
	pnew->link = NULL;
	strcpy(pnew->arg, arg);
	if (hn_head == NULL)
	{
		hn_head = pnew;
		return;
	}
	hinptr p = hn_head;
	while (p->link)
		p = p->link;
	p->link = pnew;

}

//free all of dynamic memories in history linked list
void clear_history()
{
	hinptr p = hn_head;

	while (p)
	{
		hinptr next = p->link;
		free(p);
		p = next;
	}
}

//execute history command
//show all history in its linked-list
void cmd_history()
{
	if(history_cnt == 1)
		return;
	
	int i = 1;
	for (hinptr p = hn_head; p; p = p->link, i++)
	{
		printf("%d %s", i, p->arg);
	}
}

//execute help command
//show all command name with each right format
void cmd_help()
{
	printf("h[elp]\n");
	printf("d[ir]\n");
	printf("q[uit]\n");
	printf("hi[story]\n");
	printf("du[mp] [start, end]\n");
	printf("e[dit] address, value\n");
	printf("f[ill] start, end, value\n");
	printf("reset\n");
	printf("opcode mnemonic\n");
	printf("opcodelist\n");
	printf("assemble filename\n");
	printf("type filename\n");
	printf("symbol\n");
	printf("progaddr [address]\n");
	printf("loader [object filename1] [...]\n");
	printf("run\n");
	printf("bp [address] (or) bp clear (or) bp\n");
	return;
}

//execute dir command
//show all directories and executable files in current directory
//if fail to open current directory or close return error message else return 1 
int cmd_dir()
{
	char currentPath[_MAX_PATH];
	getcwd(currentPath, _MAX_PATH);

	DIR *dir_ptr;
	struct dirent *file;
	struct stat buf;

	if ((dir_ptr = opendir(currentPath)) == NULL)
		return ERROR_DIR_OPEN;

	while ((file = readdir(dir_ptr)) != NULL)
	{
		if (file->d_name[0] != '.')
		{
			lstat(file->d_name, &buf);

			if (S_ISDIR(buf.st_mode))
				printf("%s/  ", file->d_name);
			else if (S_ISREG(buf.st_mode))
				printf("%s*  ", file->d_name);
		}

	}
	printf("\n");

	if(closedir(dir_ptr) == 1)
		return ERROR_DIR_CLOSE;
	
	return 1;
}

//execute type command
//print all text in file
//if error exist return 0 else return 1
int cmd_type(char filename[])
{
	FILE* fp = fopen(filename, "r");

	if (fp == NULL)
	{
		error_print(ERROR_FILE_OPEN);
		return 0;
	}

	char buffer[FILE_BUFFER_SIZE];

	while (fgets(buffer, FILE_BUFFER_SIZE, fp) != NULL)
	{
		printf("%s", buffer);
	}

	if (fclose(fp) == EOF)
	{
		error_print(ERROR_FILE_CLOSE);
		return 0;
	}

	return 1;
}

//execute quit command
//delete all of dynamic memmories which are assigend by the program (history linked-list, hash table)
//and return 0 and terminate process
void cmd_quit()
{
	hnptr* op_table = get_op_table_addr();
	hnptr* sym_table = get_sym_table_addr();
	clear_history();
	clear_hash(op_table);
	clear_hash(sym_table);

	exit(0);
}
