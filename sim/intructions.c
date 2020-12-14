/*
intructions.c
*/


/*------------------------------------------------------------------------------------
										INCLUDE
------------------------------------------------------------------------------------*/
#include "instructions.h"

/*------------------------------------------------------------------------------------
										DEFINES
------------------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------------
										DECLARATION
------------------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------------
										IMPLEMENTATION
------------------------------------------------------------------------------------*/
//this function sign extend the value of imm
int sign_extend(int imm)
{
	int value = (0x00000FFF & imm);
	int mask = 0x00000800;
	if (mask & imm) {
		value += 0xFFFFF000;
	}
	return value;
}
// this function extracts one byte from number
unsigned int get_byte(unsigned int num, int pos)
{
	unsigned int mask = 0xf << (pos * 4);
	return ((num & mask) >> (pos * 4));
}
// put stall when the comaand is not valid
Command put_stall(Command cmd)
{
	cmd.opcode = 0;
	cmd.rd = 0;
	cmd.rs = 0;
	cmd.rt = 0;
	cmd.immiediate = 1;
	return cmd;
}

// this function creates a struct Command from a string in memory
Command line_to_command(unsigned int inst)
{
	Command cmd;
	cmd.opcode = (get_byte(inst, 7) * 16) + get_byte(inst, 6);
	cmd.rd = get_byte(inst, 5);
	cmd.rs = get_byte(inst, 4);
	cmd.rt = get_byte(inst, 3);
	cmd.immiediate = (get_byte(inst, 2) * 16 * 16) + (get_byte(inst, 1) * 16) + get_byte(inst, 0);
	//handle all out of bounds future problems
	if (cmd.opcode < 9 || cmd.opcode == 14 || cmd.opcode == 17)//if opcode arithmetic we need to check few expations
	{
		if (cmd.rd > 15 || cmd.rt > 15 || cmd.rs > 15 || cmd.rd == 1)
			cmd = put_stall(cmd);
	}
	if (cmd.opcode > 8 && cmd.opcode < 15)//if opcode branch we need to check few expations
	{
		if (cmd.rd > 15 || cmd.rt > 15 || cmd.rs > 15)
			cmd = put_stall(cmd);
	}
	if (cmd.opcode == 15)// jal; check only cmd.rd
	{
		if (cmd.rd > 15)
			cmd = put_stall(cmd);
	}
	if (cmd.opcode == 15 || cmd.opcode == 18)//if opcode sw check only registers
	{
		if (cmd.rd > 15 || cmd.rt > 15 || cmd.rs > 15)
			cmd = put_stall(cmd);
	}
	if (cmd.opcode > 20) //how to handle error opcode that not exist
		cmd = put_stall(cmd);

	return cmd;
}


int execution(int regs[], int pc, Command cmd, unsigned int* mem) {
	switch (cmd.opcode)
	{
	case 0: //add opcode
	{
		if (cmd.rd == 0 && cmd.rs == 0 && cmd.rt == 0 && cmd.immiediate == 0)
			break; // it's a stall
		else {
			add(regs, cmd);
			regs[0] = 0; // make sure $zero is zero
			pc++;
			break;
		}
	}
	case 1: //sub opcode
	{
		sub(regs, cmd);
		regs[0] = 0;
		pc++;
		break;
	}
	case 2: //and opcode
	{
		and (regs, cmd);
		regs[0] = 0;
		pc++;
		break;
	}
	case 3://or opcode
	{
		or (regs, cmd);
		regs[0] = 0;
		pc++;
		break;
	}
	case 4: //xor opcode
	{
		xor (regs, cmd);
		regs[0] = 0;
		pc++;
		break;
	}
	case 5: //mul opcode
	{
		mul(regs, cmd);
		regs[0] = 0;
		pc++;
		break;
	}
	case 6: //sll opcode
	{
		sll(regs, cmd);
		regs[0] = 0;
		pc++;
		break;
	}
	case 7: //sra opcode
	{
		sra(regs, cmd);
		regs[0] = 0;
		pc++;
		break;
	}
	case 8: //srl opcode
	{
		srl(regs, cmd);
		regs[0] = 0;
		pc++;
		break;
	}
	case 9: //beq opcode
	{
		pc = beq(regs, cmd, pc);
		regs[0] = 0;
		break;
	}
	case 10: //bne opcode
	{
		pc = bne(regs, cmd, pc);
		regs[0] = 0;
		break;
	}
	case 11: //blt opcode
	{
		pc = blt(regs, cmd, pc);
		regs[0] = 0;
		break;
	}
	case 12: //bgt opcode
	{
		pc = bgt(regs, cmd, pc);
		regs[0] = 0;
		break;
	}
	case 13: //ble opcode
	{
		pc = ble(regs, cmd, pc);
		regs[0] = 0;
		break;
	}
	case 14: //bge opcode
	{
		pc = bge(regs, cmd, pc);
		regs[0] = 0;
		break;
	}
	case 15: //jal opcode
	{
		pc = jal(regs, cmd, pc);
		regs[0] = 0;
		break;
	}
	case 16: //lw opcode
	{
		lw(regs, cmd, mem);
		regs[0] = 0;
		pc++;
		break;
	}
	case 17: //sw opcode
	{
		sw(regs, cmd, mem);
		regs[0] = 0;
		pc++;
		break;
	}
	case 18: //ll opcode
	{
		ll(regs, cmd, mem);
		regs[0] = 0;
		pc++;
		break;
	}
	case 19: //sc opcode
	{
		sc(regs, cmd, mem);
		regs[0] = 0;
		pc++;
		break;
	}
	case 20: //halt command, exit simulator
	{
		pc = -1;
		break;
	}
	}
	return pc;
}

//basic commands and instructions

//add command
void add(int* regs, Command cmd)
{
	regs[cmd.rd] = regs[cmd.rs] + regs[cmd.rt];
}

//sub command
void sub(int* regs, Command cmd)
{
	regs[cmd.rd] = regs[cmd.rs] - regs[cmd.rt];
}

// and command.
void and (int* regs, Command cmd)
{
	regs[cmd.rd] = regs[cmd.rs] & regs[cmd.rt];
}

// or command.
void or (int* regs, Command cmd)
{
	regs[cmd.rd] = regs[cmd.rs] | regs[cmd.rt];
}

// xor command.
void xor (int* regs, Command cmd)
{
	regs[cmd.rd] = regs[cmd.rs] ^ regs[cmd.rt];
}

// mul command.
void mul (int* regs, Command cmd)
{
	regs[cmd.rd] = regs[cmd.rs] * regs[cmd.rt];
}

// sll command.
void sll(int* regs, Command cmd)
{
	regs[cmd.rd] = regs[cmd.rs] << regs[cmd.rt];
}

//sra command
void sra(int* regs, Command cmd)
{
	int mask = regs[cmd.rs] >> 31 << 31 >> (regs[cmd.rt]) << 1;
	regs[cmd.rd] = mask ^ (regs[cmd.rs] >> regs[cmd.rt]);
}

//srl command
void srl(int* regs, Command cmd)
{
	regs[cmd.rd] = regs[cmd.rs] >> regs[cmd.rt];
}

//beq command
int beq(int* regs, Command cmd, int pc)
{
	if (regs[cmd.rs] == regs[cmd.rt])
		return pc = regs[cmd.rd];
	else {
		pc++;
		return pc;
	}
}

//bne command
int bne(int* regs, Command cmd, int pc)
{
	if (regs[cmd.rs] != regs[cmd.rt])
		return pc = regs[cmd.rd];
	else {
		pc++;
		return pc;
	}
}

//blt command
int blt(int* regs, Command cmd, int pc)
{
	if (regs[cmd.rs] < regs[cmd.rt])
		return pc = regs[cmd.rd];
	else {
		pc++;
		return pc;
	}
}

//bgt command
int bgt(int* regs, Command cmd, int pc)
{
	if (regs[cmd.rs] > regs[cmd.rt])
		return pc = regs[cmd.rd];
	else {
		pc++;
		return pc;
	}
}

//ble command
int ble(int* regs, Command cmd, int pc)
{
	if (regs[cmd.rs] <= regs[cmd.rt])
		return pc = regs[cmd.rd];
	else {
		pc++;
		return pc;
	}
}

//bge command
int bge(int* regs, Command cmd, int pc)
{
	if (regs[cmd.rs] >= regs[cmd.rt])
		return pc = regs[cmd.rd];
	else {
		pc++;
		return pc;
	}
}

//jal command
int jal(int* regs, Command cmd, int pc)
{
	regs[15] = pc + 1;
	return pc = regs[cmd.rd];
}

//lw command
void lw(int* regs, Command cmd, unsigned int* mem)
{
	if (regs[cmd.rs] + regs[cmd.rt] < MAIN_MEM_SIZE)
		regs[cmd.rd] = mem[regs[cmd.rs] + regs[cmd.rt]];
}

//sw command.
void sw(int* regs, Command cmd, unsigned int* mem)
{
	if (regs[cmd.rs] + regs[cmd.rt] < MAIN_MEM_SIZE)
		mem[regs[cmd.rs] + regs[cmd.rt]] = regs[cmd.rd];
}

//ll command.
void ll(int* regs, Command cmd, unsigned int* mem)
{

}

//sw command.
void sc(int* regs, Command cmd, unsigned int* mem)
{

}






