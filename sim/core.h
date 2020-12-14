#pragma once
/*
core.h
*/


/*------------------------------------------------------------------------------------
										INCLUDE
------------------------------------------------------------------------------------*/
#include "instructions.h"

/*------------------------------------------------------------------------------------
										DEFINES
------------------------------------------------------------------------------------*/
#define I_MEM_SIZE 1024
#define DSRAM_SIZE 256
#define TSRAM_SIZE 256
#define MAIN_MEM_SIZE 1048576 // check for updates
#define MAX_LINE_TRACE 150

typedef struct _bus {
	unsigned int bus_origid; //3 bits. 0-3 cores; 4 mem
	unsigned int bus_cmd;	 //2 bits
	unsigned int bus_addr;	 //20 bits of address
	unsigned int bus_data;	 //32 bits of data. 1 word
}Bus;

typedef struct _tsram {
	unsigned int msi;		 //2 bits 0-i 1-s 2-m
	unsigned int tag;		 //12 bits
}TSRAM;

//holds all the information about the commands inside the pipeline
typedef struct _pipe {
	int core_id;
	char* IF;
	char* ID;
	char* EX;
	char* MEM;
	char* WB;
}PIPE, * PIPE_ptr;

/*------------------------------------------------------------------------------------
										DECLARATION
------------------------------------------------------------------------------------*/
////run all the 4 cores parallel
//void manage_cores(int pc, int core_id, int inst, unsigned int* imem, int* regs, FILE* fp_trace);

int core_execution(int cycle, int pc, int core_id, unsigned int* imem, int* regs, PIPE_ptr pipe, FILE* fp_trace);

void snoop_bus(Bus last_bus, TSRAM ts);

PIPE_ptr init_pipe(int core_id);

void  initilize_pipelines(PIPE_ptr pipe_0, PIPE_ptr pipe_1, PIPE_ptr pipe_2, PIPE_ptr pipe_3);


