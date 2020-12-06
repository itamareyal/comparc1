/*
intructions.c
*/


/*------------------------------------------------------------------------------------
										INCLUDE
------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------
										DEFINES
------------------------------------------------------------------------------------*/
typedef struct _command {
	unsigned int opcode;
	unsigned int rd;
	unsigned int rs;
	unsigned int rt;
	unsigned int immiediate;
}Command;


/*------------------------------------------------------------------------------------
										DECLARATION
------------------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------------
										IMPLEMENTATION
------------------------------------------------------------------------------------*/



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
	if (cmd.opcode < 7 || cmd.opcode == 14 || cmd.opcode == 17)//if opcode arithmetic we need to check few expations
	{
		if (cmd.rd > 15 || cmd.rt > 15 || cmd.rs > 15 || cmd.rd == 1)
			cmd = put_stall(cmd);
	}
	if (cmd.opcode > 6 && cmd.opcode < 13)//if opcode branch we need to check few expations
	{
		if (cmd.rd > 15 || cmd.rt > 15 || cmd.rs > 15)
			cmd = put_stall(cmd);
	}
	if (cmd.opcode == 13)// check only cmd.rd
	{
		if (cmd.rd > 15)
			cmd = put_stall(cmd);
	}
	if (cmd.opcode == 15 || cmd.opcode == 18)//if opcode sw check only registers
	{
		if (cmd.rd > 15 || cmd.rt > 15 || cmd.rs > 15)
			cmd = put_stall(cmd);
	}
	if (cmd.opcode > 19) //how to handle error opcode that not exist
		cmd = put_stall(cmd);

	return cmd;
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
	if (regs[cmd.rs] + regs[cmd.rt] < MEM_SIZE)
		regs[cmd.rd] = mem[regs[cmd.rs] + regs[cmd.rt]];
}

//sw command.
void sw(int* regs, Command cmd, unsigned int* mem)
{
	if (regs[cmd.rs] + regs[cmd.rt] < MEM_SIZE)
		mem[regs[cmd.rs] + regs[cmd.rt]] = regs[cmd.rd];
}

//reti command
int reti(int* io_regs, int pc, int* reti_flag)// flag to know the status of reti 
{
	if (*reti_flag != 0)
		*reti_flag = 0;
	else
		*reti_flag = 1;
	return io_regs[7];
}

//in command
void in(int* io_regs, int* regs, Command cmd)
{
	if (regs[cmd.rs] + regs[cmd.rt] < 18)
		regs[cmd.rd] = io_regs[regs[cmd.rs] + regs[cmd.rt]];
}

// out command
void out(int* io_regs, int* regs, Command cmd, int* disk, int* mem)
{
	if (regs[cmd.rs] + regs[cmd.rt] < 18)
		if ((regs[cmd.rs] + regs[cmd.rt]) == 14)
		{
			io_regs[14] = regs[cmd.rd];
			disk_handel(disk, io_regs, mem);
		}
		else
			io_regs[regs[cmd.rs] + regs[cmd.rt]] = regs[cmd.rd];
}


int execution(int regs[], int io_regs[], int pc, Command cmd, unsigned int* mem, int* disk, int* reti_flag) {
	switch (cmd.opcode)
	{
	case 0: //add opcode
	{
		if (cmd.rd == 0 && cmd.rs == 0 && cmd.rt == 0 && cmd.immiediate == 0)
			break;
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
	case 4: //sll opcode
	{
		sll(regs, cmd);
		regs[0] = 0;
		pc++;
		break;
	}
	case 5: //sra opcode
	{
		sra(regs, cmd);
		regs[0] = 0;
		pc++;
		break;
	}
	case 6: //srl opcode***************
	{
		srl(regs, cmd);
		regs[0] = 0;
		pc++;
		break;
	}
	case 7: //beq opcode
	{
		pc = beq(regs, cmd, pc);
		regs[0] = 0;
		break;
	}
	case 8: //bne opcode
	{
		pc = bne(regs, cmd, pc);
		regs[0] = 0;
		break;
	}
	case 9: //blt opcode
	{
		pc = blt(regs, cmd, pc);
		regs[0] = 0;
		break;
	}
	case 10: //bgt opcode
	{
		pc = bgt(regs, cmd, pc);
		regs[0] = 0;
		break;
	}
	case 11: //ble opcode
	{
		pc = ble(regs, cmd, pc);
		regs[0] = 0;
		break;
	}
	case 12: //bge opcode
	{
		pc = bge(regs, cmd, pc);
		regs[0] = 0;
		break;
	}
	case 13: //jal opcode
	{
		pc = jal(regs, cmd, pc);
		regs[0] = 0;
		break;
	}
	case 14: //lw opcode
	{
		lw(regs, cmd, mem);
		regs[0] = 0;
		pc++;
		break;
	}
	case 15: //sw opcode
	{
		sw(regs, cmd, mem);
		regs[0] = 0;
		pc++;
		break;
	}
	case 16: //reti command
	{
		pc = reti(io_regs, pc, reti_flag);
		break;
	}
	case 17://in command
	{
		in(io_regs, regs, cmd);
		regs[0] = 0;
		pc++;
		break;
	}
	case 18://out command
	{
		out(io_regs, regs, cmd, disk, mem);
		regs[0] = 0;
		pc++;
		break;
	}
	case 19: //halt command, we need to exit simulator
	{
		pc = -1;
		break;
	}
	}
	return pc;
}

