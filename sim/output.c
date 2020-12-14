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

void create_line_for_trace(char line_for_trace[], int regs[], int pc, unsigned int inst, int imm)
{
	int i;
	char inst_line[BUFFER_MAX_SIZE];
	char pc_char[MAX_PC_CHAR] = { 0 };
	char temp_reg_char[BUFFER_MAX_SIZE] = { 0 };
	sprintf_s(pc_char, MAX_PC_CHAR, "%08X", pc);
	sprintf_s(inst_line, BUFFER_MAX_SIZE, "%08X", inst);
	sprintf_s(line_for_trace, BUFFER_MAX_SIZE, pc_char); //add pc to line
	sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");
	sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, inst_line); //add opcode to line
	sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");

	for (i = 0; i < 15; i++) { //add registers to line
		int temp_reg = 0;
		if (i == 1)// for imm
		{
			sprintf_s(temp_reg_char, "%08X", sign_extend(imm));//change to hex
			sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE,temp_reg_char);//add to line
			sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE," ");
			continue;
		}
		if (regs[i] < 0)
			temp_reg = neg_to_pos(regs[i]);
		else
			temp_reg = regs[i];
		sprintf_s(temp_reg_char, "%08X", temp_reg);//change to hex
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
