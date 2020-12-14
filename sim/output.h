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

/*------------------------------------------------------------------------------------
										DEFINES
------------------------------------------------------------------------------------*/
#define I_MEM_SIZE 1024
#define BUFFER_MAX_SIZE 9
#define MAX_PC_CHAR 10



/*------------------------------------------------------------------------------------
										DECLARATION
------------------------------------------------------------------------------------*/
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




