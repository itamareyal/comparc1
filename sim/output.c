/*
output.c
*/


/*------------------------------------------------------------------------------------
										INCLUDE
------------------------------------------------------------------------------------*/


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
	FILE* fp_regout;

	fp_regout = fopen(file_name, "w"); // open new file
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
	FILE* fp_memout;
	fp_memout = fopen(file_name, "w"); // open new file
	if (fp_memout == NULL) // handle error
	{
		printf("error opening file");
		exit(1);
	}
	for (int i = 0; i < MEM_SIZE; i++) // print memory to file
	{
		fprintf(fp_memout, "%08X\n", *mem);
		mem++;
	}
	fclose(fp_memout); // close file
}

void create_line_for_trace(char line_for_trace[], int regs[], int pc, unsigned int inst, int imm)
{
	int i;
	char inst_line[9];
	char pc_char[10] = { 0 };
	char temp_reg_char[9] = { 0 };
	sprintf(pc_char, "%08X", pc);
	sprintf(inst_line, "%08X", inst);
	sprintf(line_for_trace, pc_char); //add pc to line
	sprintf(line_for_trace + strlen(line_for_trace), " ");
	sprintf(line_for_trace + strlen(line_for_trace), inst_line); //add opcode to line
	sprintf(line_for_trace + strlen(line_for_trace), " ");

	for (i = 0; i < 15; i++) { //add registers to line
		int temp_reg = 0;
		if (i == 1)// for imm
		{
			sprintf(temp_reg_char, "%08X", sign_extend(imm));//change to hex
			sprintf(line_for_trace + strlen(line_for_trace), temp_reg_char);//add to line
			sprintf(line_for_trace + strlen(line_for_trace), " ");
			continue;
		}
		if (regs[i] < 0)
			temp_reg = neg_to_pos(regs[i]);
		else
			temp_reg = regs[i];
		sprintf(temp_reg_char, "%08X", temp_reg);//change to hex
		sprintf(line_for_trace + strlen(line_for_trace), temp_reg_char);//add to line
		sprintf(line_for_trace + strlen(line_for_trace), " ");
	}

	//add last register to line (without space at the end)
	int temp_reg = 0;
	if (regs[i] < 0)
		temp_reg = neg_to_pos(regs[i]);
	else
		temp_reg = regs[i];
	sprintf(temp_reg_char, "%.8X", temp_reg);
	sprintf(line_for_trace + strlen(line_for_trace), temp_reg_char);
}
