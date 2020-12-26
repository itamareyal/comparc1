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

#include "HardCodedData.h"

/*------------------------------------------------------------------------------------
										DEFINES
------------------------------------------------------------------------------------*/

typedef struct _bus {
	unsigned int bus_origid; //3 bits. 0-3 cores; 4 mem
	unsigned int bus_cmd;	 //2 bits
	unsigned int bus_addr;	 //20 bits of address
	unsigned int bus_data;	 //32 bits of data. 1 word
	int prev_cmd;
	int flush_cycle;
	int data_owner;
	int data_destination;
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

typedef struct _stat {
	unsigned int cycles; 
	unsigned int instructions;	 
	unsigned int read_hit;	 
	unsigned int write_hit;	 
	unsigned int read_miss;
	unsigned int write_miss;
	unsigned int decode_stall;
	unsigned int mem_stall;

}STAT, * STAT_ptr;

typedef struct _command {
	unsigned int opcode;
	unsigned int rd;
	unsigned int rs;
	unsigned int rt;
	unsigned int immiediate;
	unsigned int core_id;
}Command;

/*------------------------------------------------------------------------------------
										DECLARATION
------------------------------------------------------------------------------------*/

typedef struct _command ;
////run all the 4 cores parallel
//void manage_cores(int pc, int core_id, int inst, unsigned int* imem, int* regs, FILE* fp_trace);


//execution of one core.
int core_execution(int* cycle, int pc, int core_id, unsigned int* imem, int* regs, 
	PIPE_ptr pipe, FILE* fp_trace, BUS_ptr last_bus, unsigned int* dsram,
	TSRAM_ptr tsram[], STAT_ptr stat);

//function for the snoop between the cores.
void snoop_bus(BUS_ptr last_bus, TSRAM_ptr tsram[], int* cycle, int core_id, unsigned int* dsram);

// execution of all the commands except the flush
void execution_bus(BUS_ptr last_bus, int* cycle, unsigned int mem[]);

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

//responsible to open all trace files safety
void create_line_for_trace(char line_for_trace[], int regs[], int pc, int cycle, PIPE_ptr pipe);

//function for initilize the STAT structure for all the cores.
STAT_ptr initilize_STAT();

//init all stat of all cores
void  initilize_all_stats(STAT_ptr stat_0, STAT_ptr stat_1, STAT_ptr stat_2, STAT_ptr stat_3);

int sign_extend(int imm);

unsigned int get_byte(unsigned int num, int pos);

Command put_stall(Command cmd, int core_id);

Command line_to_command(unsigned int inst, int core_id);

int execution(int regs[], int pc, Command cmd, unsigned int* mem, BUS_ptr last_bus,
	unsigned int* dsram, TSRAM_ptr tsram[], STAT_ptr stat, PIPE_ptr pipe, int* cycle);

void add(int* regs, Command cmd);
void sub(int* regs, Command cmd);
void and (int* regs, Command cmd);
void or (int* regs, Command cmd);
void xor (int* regs, Command cmd);
void mul(int* regs, Command cmd);
void sll(int* regs, Command cmd);
void sra(int* regs, Command cmd);
void srl(int* regs, Command cmd);
int beq(int* regs, Command cmd, int pc);
int bne(int* regs, Command cmd, int pc);
int blt(int* regs, Command cmd, int pc);
int bgt(int* regs, Command cmd, int pc);
int ble(int* regs, Command cmd, int pc);
int bge(int* regs, Command cmd, int pc);
int jal(int* regs, Command cmd, int pc);
void lw(int* regs, Command cmd, unsigned int* mem);
void sw(int* regs, Command cmd, unsigned int* mem);
void ll(int* regs, Command cmd, unsigned int* mem);
void sc(int* regs, Command cmd, unsigned int* mem);
