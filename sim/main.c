/*
main.c
*/


/*------------------------------------------------------------------------------------
										INCLUDE
------------------------------------------------------------------------------------*/
#include "input.h"
#include "core.h"
#include "output.h"


/*------------------------------------------------------------------------------------
										DEFINES
------------------------------------------------------------------------------------*/
#define NUMBER_REGISTER_SIZE 16

/*------------------------------------------------------------------------------------
										DECLARATION
------------------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------------
										IMPLEMENTATION
------------------------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
	// initiali registers for each core
	int regs_0[NUMBER_REGISTER_SIZE] = { 0 };
	int regs_1[NUMBER_REGISTER_SIZE] = { 0 };
	int regs_2[NUMBER_REGISTER_SIZE] = { 0 };
	int regs_3[NUMBER_REGISTER_SIZE] = { 0 };

	// initialize instruction memory for each core
	unsigned int imem_0[I_MEM_SIZE] = { 0 }; 
	unsigned int imem_1[I_MEM_SIZE] = { 0 }; 
	unsigned int imem_2[I_MEM_SIZE] = { 0 }; 
	unsigned int imem_3[I_MEM_SIZE] = { 0 }; 
	
	// initialize DSRAM for each core
	unsigned int dsram_0[DSRAM_SIZE] = { 0 }; 
	unsigned int dsram_1[DSRAM_SIZE] = { 0 }; 
	unsigned int dsram_2[DSRAM_SIZE] = { 0 }; 
	unsigned int dsram_3[DSRAM_SIZE] = { 0 }; 

	// initialize TSRAM for each core
	unsigned int tsram_0[TSRAM_SIZE] = { 0 }; 
	unsigned int tsram_1[TSRAM_SIZE] = { 0 }; 
	unsigned int tsram_2[TSRAM_SIZE] = { 0 }; 
	unsigned int tsram_3[TSRAM_SIZE] = { 0 };

	// initialize main memory
	unsigned int mem[MAIN_MEM_SIZE] = { 0 };

	Bus* last_bus; // holds last trans on the bus for snooping at next iteration
	int bus_free = 0; // 0-free for trans; 1-busy, wait for flush

	// initialize trace file for each of the cores and also one bus trace
	FILE* core_0_trace = NULL;
	FILE* core_1_trace = NULL;
	FILE* core_2_trace = NULL;
	FILE* core_3_trace = NULL;
	FILE* bus_trace = NULL;

	// initialize cycle counter and 4 pc for each of the 4 cores.
	int cycle = 0;
	int pc_0 = 0;
	int pc_1 = 0;
	int pc_2 = 0;
	int pc_3 = 0;

	// initialize pipeline for each of the cores
	PIPE_ptr pipe_0 = NULL;
	PIPE_ptr pipe_1 = NULL;
	PIPE_ptr pipe_2 = NULL;
	PIPE_ptr pipe_3 = NULL;

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

	initilize_pipelines(pipe_0, pipe_1, pipe_2, pipe_3);

	// multi core execution loop. exits when all cores are done. 
	while ((pc_0 != -1) && (pc_1 != -1) && (pc_2 != -1) && (pc_3 != -1)) {
		// execute for each core
		pc_0 = core_execution(cycle, pc_0, 0, imem_0, regs_0,pipe_0, core_0_trace);
		pc_1 = core_execution(cycle, pc_1, 1, imem_1, regs_1, pipe_1, core_1_trace);
		pc_2 = core_execution(cycle, pc_2, 2, imem_2, regs_2, pipe_2, core_2_trace);
		pc_3 = core_execution(cycle, pc_3, 3, imem_3, regs_3, pipe_3, core_3_trace);
		cycle++;
		// execute single bus transaction (if called)

		// write to live trace files
	}
	close_trace_files(core_0_trace, core_1_trace, core_2_trace, core_3_trace, bus_trace);
	// write memout, regout x4, dsram x4, tsram x4, stats x4

	//close all files
	return 0;
}

