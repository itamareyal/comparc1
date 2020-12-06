/*
input.h
*/


/*------------------------------------------------------------------------------------
										INCLUDE
------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*------------------------------------------------------------------------------------
										DEFINES
------------------------------------------------------------------------------------*/
#define SIZE_ARGV 28
#define SIZE_MEM_MAX_LINE 21


/*------------------------------------------------------------------------------------
										DECLARATION
------------------------------------------------------------------------------------*/
char** check_args(int argc, char* argv[]);

FILE* open_file(FILE* fp, char* address, char mode);

// open imem & mem files and read them to arrays
int open_mem_files(int argc, char* argv[], unsigned int imem_0[], unsigned int imem_1[], unsigned int imem_2[], unsigned int imem_3[], unsigned int mem[]);

int read_memin_imem(unsigned int* mem, char* address);

// open trace file in w mode
int open_trace_files(char* args[], FILE* core_0_trace, FILE* core_1_trace, FILE* core_2_trace, FILE* core_3_trace, FILE* bus_trace);

/*------------------------------------------------------------------------------------
										IMPLEMENTATION
------------------------------------------------------------------------------------*/
