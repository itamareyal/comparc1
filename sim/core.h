#pragma once
/*
core.h
*/
/*------------------------------------------------------------------------------------
										INCLUDE
------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "instructions.h"
#include "HardCodedData.h"

/*------------------------------------------------------------------------------------
										DEFINES
------------------------------------------------------------------------------------*/


typedef struct _bus {
	unsigned int bus_origid; //3 bits. 0-3 cores; 4 mem
	unsigned int bus_cmd;	 //2 bits
	unsigned int bus_addr;	 //20 bits of address
	unsigned int bus_data;	 //32 bits of data. 1 word
	int flush_cycle;
	int data_owner;
	int bus_busy;
}BUS, *BUS_ptr;

typedef struct _tsram {
	unsigned int msi;		 //2 bits 0-i 1-s 2-m
	unsigned int tag;		 //12 bits
}TSRAM, *TSRAM_ptr;

//holds all the information about the commands inside the pipeline
typedef struct _pipe {
	int core_id;
	int IF;
	int ID;
	int EX;
	int MEM;
	int WB;
}PIPE, * PIPE_ptr;

/*------------------------------------------------------------------------------------
										DECLARATION
------------------------------------------------------------------------------------*/
////run all the 4 cores parallel
//void manage_cores(int pc, int core_id, int inst, unsigned int* imem, int* regs, FILE* fp_trace);


//execution of one core.
int core_execution(int* cycle, int pc, int core_id, unsigned int* imem, int* regs, 
	PIPE_ptr pipe, FILE* fp_trace, BUS_ptr last_bus, unsigned int* dsram, TSRAM_ptr tsram[]);

//function for the snoop between the cores.
void snoop_bus(BUS_ptr last_bus, TSRAM_ptr tsram[], int* cycle);

// execution of all the commands except the flush
void execution_bus(BUS_ptr last_bus, TSRAM_ptr tsram);

// return the data requested by a core in read or readx
void check_flush(BUS_ptr last_bus, TSRAM_ptr tsram, unsigned int mem[], int* bus_busy, int* cycle);

//initilize one pipeline
void init_pipe(int core_id, PIPE_ptr pipe);

//initiilize all the pipelines of all the cores
void  initilize_pipelines(PIPE_ptr pipe_0, PIPE_ptr pipe_1, PIPE_ptr pipe_2, PIPE_ptr pipe_3);

//each cycle update the pipline with the PC
void update_pipeline(PIPE_ptr pipe, int pc);

//function for initilize the bus for all the cores.
BUS_ptr initilize_bus();

//get the tag from the address in memory
int get_tag(int address);

//get the tag from the tag in memory
int get_index(int address);

