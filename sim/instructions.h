#pragma once
/*
instructions.h
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
int sign_extend(int imm);

unsigned int get_byte(unsigned int num, int pos);

Command put_stall(Command cmd, int core_id);

Command line_to_command(unsigned int inst,int core_id);

int execution(int regs[], int pc, Command cmd, unsigned int* mem);

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
