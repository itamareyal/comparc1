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
void create_regout(int regs[], char file_name[]);

void create_memout(unsigned int* mem, char file_name[]);

void create_line_for_trace(char line_for_trace[], int regs[], int pc, unsigned int inst, PIPE_ptr pipe);

int neg_to_pos(signed int num);


