/*
input.c
*/


/*------------------------------------------------------------------------------------
										INCLUDE
------------------------------------------------------------------------------------*/
#include "input.h"

/*------------------------------------------------------------------------------------
										DEFINES
------------------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------------
										DECLARATION
------------------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------------
										IMPLEMENTATION
------------------------------------------------------------------------------------*/

char ** check_args(int argc, char* argv[]) {

	if (argc == 1) { // no cmd args given
		char *args[SIZE_ARGV] = {
				"sim.exe",
				"imem0.txt", "imem1.txt", "imem2.txt", "imem3.txt",
				"memin.txt", "memout.txt",
				"regout0.txt", "regout1.txt", "regout2.txt", "regout3.txt",
				"core0trace.txt", "core1trace.txt", "core2trace.txt", "core3trace.txt",
				"bustrace.txt",
				"dsram0.txt", "dsram1.txt", "dsram2.txt", "dsram3.txt",
				"tsram0.txt", "tsram1.txt", "tsram2.txt", "tsram3.txt",
				"stats0.txt", "stats1.txt", "stats2.txt", "stats3.txt"
	};
		return args;
	}

	else { //cmd line args given
		return argv;
	}

}

FILE* open_file(FILE* fp, char* address, char mode) {
	if (NULL == fopen_s(fp, address, mode)) {
		printf("Error opening file %s\n", address);
		exit(1);
	}
	return fp;
}

// open imem & mem files and read them to arrays
int open_mem_files(int argc, char* argv[], unsigned int imem_0[], unsigned int imem_1[], unsigned int imem_2[], unsigned int imem_3[], unsigned int mem[]) {
	
	char* args[SIZE_ARGV] = check_args(argc, argv);
	read_memin_imem(imem_0, args[1]);
	read_memin_imem(imem_1, args[2]);
	read_memin_imem(imem_2, args[3]);
	read_memin_imem(imem_3, args[4]);
	read_memin_imem(mem, args[5]);

	return 0;
}


int open_trace_files(char* args[], FILE *core_0_trace, FILE* core_1_trace, FILE* core_2_trace, FILE* core_3_trace, FILE* bus_trace) {
	char mode = "w";
	open_file(core_0_trace, args[11], mode);
	open_file(core_1_trace, args[12], mode);
	open_file(core_2_trace, args[13], mode);
	open_file(core_3_trace, args[14], mode);
	open_file(bus_trace, args[15], mode);

	return 0;
}

int read_memin_imem(unsigned int* mem, char* address)
{
	FILE* fp = NULL;
	char mode = "r";
	open_file(fp, address, mode);

	// read memin file line by line and turn it into array
	char line[SIZE_MEM_MAX_LINE];
	int i = 0;
	while (!feof(fp) && fgets(line, SIZE_MEM_MAX_LINE, fp))
	{
		if (strcmp(line, "\n") == 0 || strcmp(line, "\0") == 0) // ignore white spaces
			continue;
		mem[i] = strtol(line, NULL, 16);
		i++;
	}
	fclose(fp); // close file
	return 0;
}

