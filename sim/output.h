#pragma once
/*
output.h

responsible on all the outputs of the program
*/


/*------------------------------------------------------------------------------------
										INCLUDE
------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core.h"
#include "HardCodedData.h"

/*------------------------------------------------------------------------------------
										DEFINES
------------------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------------
										DECLARATION
------------------------------------------------------------------------------------*/
//create all the output files that not have to update during the execution.
void write_output_files(char **args,int *regs_0,int *regs_1,int *regs_2,int *regs_3, 
	unsigned int *dsram_0, unsigned int *dsram_1, unsigned int*dsram_2, unsigned int *dsram_3, 
	TSRAM_ptr tsram_0[], TSRAM_ptr tsram_1[], TSRAM_ptr tsram_2[], TSRAM_ptr tsram_3[]
	, unsigned int *mem);

//create the tsram and dsram output file.
void create_dsram_output(unsigned int* dsram, char file_name[]);

//create the tsram and dsram output file.
void create_tsram_output(TSRAM_ptr tsram[], char file_name[]);

//function to create the regout file
void create_regout(int regs[], char file_name[]);

//function to create the memout file
void create_memout(unsigned int* mem, char file_name[]);

//responsible to open all trace files safety
void create_line_for_trace(char line_for_trace[], int regs[], int pc, unsigned int inst, PIPE_ptr pipe);

//responsible to close all the trace files
void close_trace_files(FILE* core_0_trace, FILE* core_1_trace, FILE* core_2_trace, FILE* core_3_trace, FILE* bus_trace);

//A function that converts a negative number to positive in 2's compliment
int neg_to_pos(signed int num);




