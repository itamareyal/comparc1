/*
output.c
*/


/*------------------------------------------------------------------------------------
										INCLUDE
------------------------------------------------------------------------------------*/
#include "output.h"

/*------------------------------------------------------------------------------------
										DEFINES
------------------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------------
										DECLARATION
------------------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------------
										IMPLEMENTATION
------------------------------------------------------------------------------------*/
void write_output_files(char** args, int* regs_0, int* regs_1, int* regs_2, int* regs_3,
	unsigned int* dsram_0, unsigned int* dsram_1, unsigned int* dsram_2, unsigned int* dsram_3,
	unsigned int* tsram_0, unsigned int* tsram_1, unsigned int* tsram_2, unsigned int* tsram_3
	, unsigned int* mem)
{
	create_memout(mem, args[6]);
	create_regout(regs_0, args[7]);
	create_regout(regs_1, args[8]);
	create_regout(regs_2, args[9]);
	create_regout(regs_3, args[10]);
	create_dsram_output(dsram_0, args[16]);
	create_dsram_output(dsram_1, args[17]);
	create_dsram_output(dsram_2, args[18]);
	create_dsram_output(dsram_3, args[19]);
	create_tsram_output(tsram_0, args[20]);
	create_tsram_output(tsram_1, args[21]);
	create_tsram_output(tsram_2, args[22]);
	create_tsram_output(tsram_3, args[23]);
	//still remain to build a function for the stats
}

void create_dsram_output(unsigned int* dsram, char file_name[]) {
	FILE* fp_memout = NULL;
	fopen_s(fp_memout, file_name, "w"); // open new file
	if (fp_memout == NULL) // handle error
	{
		printf("error opening file");
		exit(1);
	}
	for (int i = 0; i < DSRAM_TSRAM_SIZE; i++) // print memory to file
	{
		fprintf(fp_memout, "%08X\n", *dsram);
		dsram++;
	}
	fclose(fp_memout); // close file
}

void create_tsram_output(unsigned int* tsram, char file_name[]) {
	FILE* fp_memout = NULL;
	fopen_s(fp_memout, file_name, "w"); // open new file
	if (fp_memout == NULL) // handle error
	{
		printf("error opening file");
		exit(1);
	}
	for (int i = 0; i < DSRAM_TSRAM_SIZE; i++) // print memory to file
	{
		fprintf(fp_memout, "%04X\n", *tsram);
		tsram++;
	}
	fclose(fp_memout); // close file
}

// this function creates regout file
void create_regout(int regs[], char file_name[]) {
	FILE* fp_regout=NULL;

	fopen_s(fp_regout, file_name, "w"); // open new file
	if (fp_regout == NULL) // handle error
	{
		printf("error opening file");
		exit(1);
	}
	for (int i = 2; i <= 15; i++) // print registers to file
	{
		fprintf(fp_regout, "%08X\n", regs[i]);
	}
	fclose(fp_regout); // close file
}

void create_memout(unsigned int* mem, char file_name[]) {
	FILE* fp_memout=NULL;
	fopen_s(fp_memout, file_name, "w"); // open new file
	if (fp_memout == NULL) // handle error
	{
		printf("error opening file");
		exit(1);
	}
	for (int i = 0; i < I_MEM_SIZE; i++) // print memory to file
	{
		fprintf(fp_memout, "%08X\n", *mem);
		mem++;
	}
	fclose(fp_memout); // close file
}

void create_line_for_trace(char line_for_trace[], int regs[], int pc, unsigned int cycle, PIPE_ptr pipe)
{
	//initinlize parameters
	int i;
	char inst_line[BUFFER_MAX_SIZE];
	char cycle_char[MAX_PC_CHAR] = { 0 };
	char temp_reg_char[BUFFER_MAX_SIZE] = { 0 };

	//add cycle and fetch to the output line
	sprintf_s(cycle_char, MAX_PC_CHAR, "%08X", cycle);
	//sprintf_s(inst_line, BUFFER_MAX_SIZE, "%08X", pc);
	sprintf_s(line_for_trace, BUFFER_MAX_SIZE, cycle_char); //add pc to line
	sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");
	//sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, inst_line); //add opcode to line
	//sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");
	//handle pipeline. add each value manuelly
	strcpy_s(temp_reg_char, BUFFER_MAX_SIZE, pipe->IF);
	sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, temp_reg_char);//add to line
	sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");
	strcpy_s(temp_reg_char, BUFFER_MAX_SIZE, pipe->ID);
	sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, temp_reg_char);//add to line
	sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");
	strcpy_s(temp_reg_char, BUFFER_MAX_SIZE, pipe->EX);
	sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, temp_reg_char);//add to line
	sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");
	strcpy_s(temp_reg_char, BUFFER_MAX_SIZE,pipe->MEM);
	sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, temp_reg_char);//add to line
	sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");
	strcpy_s(temp_reg_char, BUFFER_MAX_SIZE, pipe->WB);
	sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, temp_reg_char);//add to line
	sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");
	
	//add registers to line from R2 to R14
	for (i = 1; i < 15; i++) { 
		int temp_reg = 0;
		if (regs[i] < 0)
			temp_reg = neg_to_pos(regs[i]);
		else
			temp_reg = regs[i];
		sprintf_s(temp_reg_char, BUFFER_MAX_SIZE, "%08X", temp_reg);//change to hex
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, temp_reg_char);//add to line
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");
	}

	//add last register to line (without space at the end)
	int temp_reg = 0;
	if (regs[i] < 0)
		temp_reg = neg_to_pos(regs[i]);
	else
		temp_reg = regs[i];
	sprintf_s(temp_reg_char, BUFFER_MAX_SIZE, "%.8X", temp_reg);
	sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, temp_reg_char);
}

//A function that converts a negative number to positive in 2's compliment
int neg_to_pos(signed int num)
{
	num = abs(num);
	signed int mask = 0xffffffff;
	num = num ^ mask; // invert all bits
	num++; // add 1 as in 2's comp
	return num;
}
