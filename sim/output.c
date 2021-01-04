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
	TSRAM tsram_0[], TSRAM tsram_1[], TSRAM tsram_2[], TSRAM tsram_3[]
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

	fopen_s(&fp_memout, file_name, "w"); // open new file
	if (fp_memout == NULL) // handle error
	{
		printf("error opening file");
		exit(1);
	}
	for (int i = 0; i < DSRAM_SIZE; i++) // print memory to file
	{
		fprintf(fp_memout, "%08X\n", *dsram);
		dsram++;
	}
	fclose(fp_memout); // close file
}

void create_tsram_output(TSRAM tsram[], char file_name[]) {
	FILE* fp_memout = NULL;
	fopen_s(&fp_memout, file_name, "w"); // open new file
	if (fp_memout == NULL) // handle error
	{
		printf("error opening file");
		exit(1);
	}
	for (int i = 0; i < TSRAM_SIZE; i++) // print memory to file
	{
		fprintf(fp_memout, "%04X\n",tsram[i].msi+tsram[i].tag);
		tsram++;
	}
	fclose(fp_memout); // close file
}

// this function creates regout file
void create_regout(int regs[], char file_name[]) {
	FILE* fp_regout=NULL;

	fopen_s(&fp_regout, file_name, "w"); // open new file
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
	FILE* fp_memout = NULL;
	fopen_s(&fp_memout, file_name, "w"); // open new file
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

//A function that converts a negative number to positive in 2's compliment
int neg_to_pos(signed int num)
{
	num = abs(num);
	signed int mask = 0xffffffff;
	num = num ^ mask; // invert all bits
	num++; // add 1 as in 2's comp
	return num;
}
