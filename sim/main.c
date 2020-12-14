/*
main.c
*/


/*------------------------------------------------------------------------------------
										INCLUDE
------------------------------------------------------------------------------------*/
#include "input.h"
#include "core.h"

/*------------------------------------------------------------------------------------
										DEFINES
------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------
										DECLARATION
------------------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------------
										IMPLEMENTATION
------------------------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
	unsigned int imem_0[I_MEM_SIZE] = { 0 }; // initialize insruction memory 0
	unsigned int imem_1[I_MEM_SIZE] = { 0 }; // initialize insruction memory 1
	unsigned int imem_2[I_MEM_SIZE] = { 0 }; // initialize insruction memory 2
	unsigned int imem_3[I_MEM_SIZE] = { 0 }; // initialize insruction memory 3

	unsigned int dsram_0[DSRAM_SIZE] = { 0 }; // initialize DSRAM 0
	unsigned int dsram_1[DSRAM_SIZE] = { 0 }; // initialize DSRAM 1
	unsigned int dsram_2[DSRAM_SIZE] = { 0 }; // initialize DSRAM 2
	unsigned int dsram_3[DSRAM_SIZE] = { 0 }; // initialize DSRAM 3

	unsigned int tsram_0[TSRAM_SIZE] = { 0 }; // initialize TSRAM 0
	unsigned int tsram_1[TSRAM_SIZE] = { 0 }; // initialize TSRAM 1
	unsigned int tsram_2[TSRAM_SIZE] = { 0 }; // initialize TSRAM 2
	unsigned int tsram_3[TSRAM_SIZE] = { 0 }; // initialize TSRAM 3

	unsigned int mem[MAIN_MEM_SIZE] = { 0 }; // initialize main memory

	Bus* last_bus; // holds last trans on the bus for snooping at next iteration
	int bus_free = 0; // 0-free for trans; 1-busy, wait for flush

	FILE* core_0_trace = NULL;
	FILE* core_1_trace = NULL;
	FILE* core_2_trace = NULL;
	FILE* core_3_trace = NULL;
	FILE* bus_trace = NULL;

	int pc_0 = 0;
	int pc_1 = 0;
	int pc_2 = 0;
	int pc_3 = 0;

	// read mem & imem files to arrays
	if (0 != open_mem_files(argc, argv, imem_0, imem_1, imem_2, imem_3, mem)){
		printf("Error opening mem files.\n");
		exit(1);
	}

	// open live trace files for write
	if (0 != open_trace_files(argv, core_0_trace, core_1_trace, core_2_trace, core_3_trace, bus_trace)) {
		printf("Error opening trace files.\n");
		exit(1);
	}


	// multi core execution loop. exits when all cores are done. 
	while ((pc_0 != -1) && (pc_1 != -1) && (pc_2 != -1) && (pc_3 != -1)) {
		// execute for each core

		// execute single bus transaction (if called)

		// write to live trace files
	}

	// write memout, regout x4, dsram x4, tsram x4, stats x4
	//close all files
	return 0;
}

