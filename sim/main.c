/*
main.c
*/


/*------------------------------------------------------------------------------------
										INCLUDE
------------------------------------------------------------------------------------*/
#include "input.h"

/*------------------------------------------------------------------------------------
										DEFINES
------------------------------------------------------------------------------------*/
#define I_MEM_SIZE 1024
#define MAIN_MEM_SIZE 1048576 // check for updates

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
	unsigned int mem[MAIN_MEM_SIZE] = { 0 }; // initialize main memory
	
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
	if (0 != open_mem_files(argc, argv, imem_0, imem_1, imem_2, imem_3, mem)) //open memin
	{
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

	}
}

