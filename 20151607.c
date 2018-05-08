#include "main.h"
#include "argument.h"

int main()
{
	char arg[INPUT_SIZE];
	char tmp_arg[INPUT_SIZE];

	read_opcode_file();

	while(1)
	{
		printf("sicsim> ");
		fgets(arg,INPUT_SIZE, stdin);

		strcpy(tmp_arg, arg);
		refine_argument(tmp_arg, arg);
		
		if (is_help(arg, tmp_arg))
			;
		else if (is_dir(arg, tmp_arg))
			;
		else if (is_quit(arg))
			;
		else if (is_history(arg, tmp_arg))
			;
		else if (is_dump(arg, tmp_arg))
			;
		else if (is_edit(arg, tmp_arg))
			;
		else if (is_fill(arg, tmp_arg))
			;
		else if (is_reset(arg, tmp_arg))
			;
		else if (is_opcodelist(arg, tmp_arg))
			;
		else if (is_opcode_mnemonic(arg, tmp_arg))
			;
		else if (is_type(arg, tmp_arg))
			;
		else if (is_assemble(arg, tmp_arg))
			;
		else if (is_symbol(arg, tmp_arg))
			;
		else if (is_loader(arg, tmp_arg))
			;
		else if (is_progaddr(arg, tmp_arg))
			;
		else if (is_bp(arg, tmp_arg))
			;
		else if (is_run(arg, tmp_arg))
			;
		else
			error_print(ERROR_NOT_FOUND);
	}
	return 0;
}

