/*
core.c

Hold functions to dispatch and manage cores in the program
*/


/*------------------------------------------------------------------------------------
										INCLUDE
------------------------------------------------------------------------------------*/
#include "core.h"

/*------------------------------------------------------------------------------------
										DEFINES
------------------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------------
										DECLARATION
------------------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------------
										IMPLEMENTATION
------------------------------------------------------------------------------------*/
//run all the 4 cores parallel
//void manage_cores(int cycle, int pc, int core_id, int inst, unsigned int* imem, int* regs, FILE* fp_trace) {
	/*int pc_0, pc_1, pc_2, pc_3 = 0;
	core_execution(pc_0,)*/

int core_execution(int* cycle, int pc, int core_id, unsigned int *imem, int *regs,
	PIPE_ptr pipe, FILE* fp_trace, BUS_ptr last_bus, unsigned int* dsram,
	TSRAM_ptr tsram[],STAT_ptr stat) {
	if (pc == -1)
		return -1;
	int inst = imem[pc];
	Command cmd = line_to_command(inst, core_id); // create Command struct
	update_pipeline(pipe, pc);
	char line_for_trace[MAX_LINE_TRACE] = { 0 }; //create line for trace file
	create_line_for_trace(line_for_trace, regs, pc, cycle, pipe);//append to trace file
	fprintf_s(fp_trace, "%s\n", line_for_trace);
	regs[1] = sign_extend(cmd.immiediate);//first we do sign extend to immiediate
	snoop_bus(last_bus, tsram,cycle,core_id, dsram);
	pc = execution(regs, pc, cmd, imem, last_bus, dsram, tsram, stat,pipe,cycle);
	return pc;
}

void snoop_bus(BUS_ptr last_bus, TSRAM_ptr tsram[], int* cycle, int core_id, unsigned int* dsram) {
	int tag, index;
	tag = get_tag(last_bus->bus_addr);
	index = get_index(last_bus->bus_addr);
	switch (last_bus->bus_cmd) {
	case 0: // no cmd
	{
		break;
	}
	case 1: // BusRd
	{
		if (tsram[index]->msi == 0 || tsram[index]->msi == NULL)
			break;
		else if (tsram[index]->msi == 1)
			break;
		else if (tsram[index]->msi == 2) {
			last_bus->bus_data = dsram[get_index(last_bus->bus_addr)];
			last_bus->data_owner = core_id;
			tsram[index]->msi = 1;
			break;
		}
		}
	case 2 : // BusRdX
	{
		if (tsram[index]->msi == 0 || tsram[index]->msi == NULL)
			break;
		else if (tsram[index]->msi == 1) {
			tsram[index]->msi = 0;
			break;
		}
		else if (tsram[index]->msi == 2) {
			last_bus->bus_data = dsram[index];
			last_bus->data_owner = core_id;
			tsram[index]->msi = 0;
			break;
		}
	}
	case 3: // Flush
	{
		if (core_id == last_bus->data_destination) {
			if (last_bus->prev_cmd == 2)
				tsram[index]->msi = 2;
			else
				tsram[index]->msi = 1;
			tsram[index]->tag = tag;
			dsram[index] = last_bus->bus_data;
			last_bus->bus_busy = 0;//transaction complete
		}
		break;
	}
	}
}

void execution_bus(BUS_ptr last_bus, int *cycle, unsigned int mem[]) {
	if (last_bus->bus_busy == 0)
		last_bus = initilize_bus();
	else
	{
		switch (last_bus->bus_cmd) {
		case 0: // no cmd
		{
			break;
		}
		case 1: // BusRd
		{
			if (last_bus->data_owner != 4) {//another core has the data in modify mode
				last_bus->data_destination = last_bus->bus_origid;
				last_bus->bus_origid = last_bus->data_owner;
				last_bus->prev_cmd = last_bus->bus_cmd;
				last_bus->bus_cmd = 3;
				last_bus->flush_cycle = -1;
				mem[last_bus->bus_addr] = last_bus->bus_data;
			}
			else if(cycle==last_bus->flush_cycle){
				last_bus->data_destination = last_bus->bus_origid;
				last_bus->bus_origid = 4; // data will come from main mem
				last_bus->prev_cmd = last_bus->bus_cmd;
				last_bus->bus_cmd = 3;
				last_bus->flush_cycle = -1;
				last_bus->bus_data = mem[last_bus->bus_addr];
			}
		}
		case 2: // BusRdX
		{
			if (last_bus->data_owner != 4) {//another core has the data in modify mode
				last_bus->data_destination = last_bus->bus_origid;
				last_bus->bus_origid = last_bus->data_owner;
				last_bus->prev_cmd = last_bus->bus_cmd;
				last_bus->bus_cmd = 3;
				last_bus->flush_cycle = -1;
				mem[last_bus->bus_addr] = last_bus->bus_data;
			}
			else if (cycle == last_bus->flush_cycle) {
				last_bus->data_destination = last_bus->bus_origid;
				last_bus->bus_origid = 4; // data will come from main mem
				last_bus->prev_cmd = last_bus->bus_cmd;
				last_bus->bus_cmd = 3;
				last_bus->flush_cycle = -1;
				last_bus->bus_data = mem[last_bus->bus_addr];
			}
		}
		case 3: // Flush
		{
			// Flush had happened, reset bus to idle
			last_bus = initilize_bus();
		}
		}
	}
}

void init_pipe(int core_id, PIPE_ptr pipe) 
{
	/*pipe = malloc(sizeof(PIPE));
	if (pipe == NULL)
		exit(1);*/
	pipe->IF = STALL;
	pipe->ID = STALL;
	pipe->EX = STALL;
	pipe->MEM = STALL;
	pipe->WB = STALL;
	pipe->core_id = core_id;
}

void  initilize_pipelines(PIPE_ptr pipe_0, PIPE_ptr pipe_1, PIPE_ptr pipe_2, PIPE_ptr pipe_3)
{
	init_pipe(0,pipe_0);
	init_pipe(1, pipe_1);
	init_pipe(2, pipe_2);
	init_pipe(3, pipe_3);
}

void update_pipeline(PIPE_ptr pipe, int pc)
{
	pipe->WB = pipe->MEM;
	pipe->MEM = pipe->EX;
	pipe->EX = pipe->ID;
	pipe->ID = pipe->IF;
	pipe->IF = pc;
}

BUS_ptr initilize_bus() {
	BUS_ptr Bus;
	Bus = (BUS_ptr)malloc(sizeof(BUS));
	if (Bus == NULL)
		return NULL;
	Bus->bus_origid = 0;
	Bus->bus_cmd = 0;
	Bus->bus_data = 0;
	Bus->bus_addr = 0;
	Bus->data_owner = 4;//main memory need to be default
	Bus->flush_cycle = -1;//intilize to these value because we do cycle==flush cycle from start.
	Bus->bus_busy = 0;
	Bus->prev_cmd = -1;
	Bus->data_destination = -1;
	return Bus;
}

int get_tag(int address) {
	int tag = 0;
	tag = get_byte(address, 2) + get_byte(address, 3)+ get_byte(address, 4);
	return tag;
}

int get_index(int address) {
	int index = 0;
	index = get_byte(address, 0) + get_byte(address, 1);
	return index;
}
	
void create_line_for_trace(char line_for_trace[], int regs[], int pc, int cycle, PIPE_ptr pipe)
{
	//initinlize parameters
	int i;
	char cycle_char[MAX_PC_CHAR] = { 0 };
	char temp_reg_char[BUFFER_MAX_SIZE] = { 0 };
	char* stall = "---";

	//add cycle and fetch to the output line
	sprintf_s(cycle_char, MAX_PC_CHAR, "%d", cycle/4);
	sprintf_s(line_for_trace, BUFFER_MAX_SIZE, cycle_char);
	sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");

	//handle pipeline. add each value manuelly
	if (pipe->IF == STALL) {
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, stall);//add to line
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");
	}
	else {
		sprintf_s(temp_reg_char, BUFFER_MAX_SIZE, "%03X", pipe->IF);
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, temp_reg_char);//add to line
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");
	}
	if (pipe->ID == STALL) {
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, stall);//add to line
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");
	}
	else {
		sprintf_s(temp_reg_char, BUFFER_MAX_SIZE, "%03X", pipe->ID);
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, temp_reg_char);//add to line
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");
	}
	if (pipe->EX == STALL) {
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, stall);//add to line
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");
	}
	else {
		sprintf_s(temp_reg_char, BUFFER_MAX_SIZE, "%03X", pipe->EX);
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, temp_reg_char);//add to line
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");
	}
	if (pipe->MEM == STALL) {
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, stall);//add to line
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");
	}
	else {
		sprintf_s(temp_reg_char, BUFFER_MAX_SIZE, "%03X", pipe->MEM);
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, temp_reg_char);//add to line
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");
	}
	if (pipe->WB == STALL) {
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, stall);//add to line
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");
	}
	else {
		sprintf_s(temp_reg_char, BUFFER_MAX_SIZE, "%03X", pipe->WB);
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, temp_reg_char);//add to line
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");
	}

	//add registers to line from R2 to R14
	for (i = 1; i < 15; i++) {
		int temp_reg = 0;
		if (regs[i] < 0)
			temp_reg = neg_to_pos(regs[i]);
		else
			temp_reg = regs[i];
		sprintf_s(temp_reg_char, BUFFER_MAX_SIZE, "%08X", temp_reg);//change to hex
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, temp_reg_char);//add to line
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");
	}

	//add last register to line (without space at the end)
	int temp_reg = 0;
	if (regs[i] < 0)
		temp_reg = neg_to_pos(regs[i]);
	else
		temp_reg = regs[i];
	sprintf_s(temp_reg_char, BUFFER_MAX_SIZE, "%.8X", temp_reg);
	sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, temp_reg_char);
}

STAT_ptr initilize_STAT(STAT_ptr core_stat) {
	core_stat->cycles = 0;
	core_stat->decode_stall = 0;
	core_stat->instructions = 0;
	core_stat->mem_stall = 0;
	core_stat->read_hit = 0;
	core_stat->read_miss = 0;
	core_stat->write_hit = 0;
	core_stat->write_miss = 0;
}

void  initilize_all_stats(STAT_ptr stat_0, STAT_ptr stat_1, STAT_ptr stat_2, STAT_ptr stat_3) {
	initilize_STAT(stat_0);
	initilize_STAT(stat_1);
	initilize_STAT(stat_2);
	initilize_STAT(stat_3);
}
// ask for bus function and execute

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
Command put_stall(Command cmd, int core_id)
{
	cmd.opcode = 0;
	cmd.rd = 0;
	cmd.rs = 0;
	cmd.rt = 0;
	cmd.immiediate = 1;
	cmd.core_id = core_id;
	return cmd;
}

// this function creates a struct Command from a string in memory
Command line_to_command(unsigned int inst, int core_id)
{
	Command cmd;
	cmd.opcode = (get_byte(inst, 7) * 16) + get_byte(inst, 6);
	cmd.rd = get_byte(inst, 5);
	cmd.rs = get_byte(inst, 4);
	cmd.rt = get_byte(inst, 3);
	cmd.immiediate = (get_byte(inst, 2) * 16 * 16) + (get_byte(inst, 1) * 16) + get_byte(inst, 0);
	cmd.core_id = core_id;
	//handle all out of bounds future problems
	if (cmd.opcode < 9 || cmd.opcode == 14 || cmd.opcode == 17)//if opcode arithmetic we need to check few expations
	{
		if (cmd.rd > 15 || cmd.rt > 15 || cmd.rs > 15 || cmd.rd == 1)
			cmd = put_stall(cmd, core_id);
	}
	if (cmd.opcode > 8 && cmd.opcode < 15)//if opcode branch we need to check few expations
	{
		if (cmd.rd > 15 || cmd.rt > 15 || cmd.rs > 15)
			cmd = put_stall(cmd, core_id);
	}
	if (cmd.opcode == 15)// jal; check only cmd.rd
	{
		if (cmd.rd > 15)
			cmd = put_stall(cmd, core_id);
	}
	if (cmd.opcode > 20) //how to handle error opcode that not exist
		cmd = put_stall(cmd, core_id);

	return cmd;
}


int execution(int regs[], int pc, Command cmd, unsigned int* mem, BUS_ptr last_bus, unsigned int* dsram,
	TSRAM_ptr tsram[], STAT_ptr stat, PIPE_ptr pipe,int* cycle) {
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
		//invalid data (cache miss)
		if (tsram[get_index(regs[cmd.rs] + regs[cmd.rt])]->msi == 0) {
			stat->read_miss += 1;
			if (last_bus->bus_busy == 1) {
				
				//put_stall() need to remember to count the stalls.
				return pc;
			}
			else{
				last_bus->bus_origid = pipe->core_id;
				last_bus->bus_busy = 1;//start transaction
				last_bus->bus_cmd = 1;//bus read
				last_bus->flush_cycle = cycle + 64;
				last_bus->bus_addr = regs[cmd.rs] + regs[cmd.rt];
			}
			pc++;
			break;
		}
		//cache hit
		else if (tsram[get_index(regs[cmd.rs] + regs[cmd.rt])]->tag == regs[cmd.rs] + regs[cmd.rt]){// i have the data in my dsram so i can get it
			lw(regs, cmd, dsram);
			stat->read_hit += 1;
			regs[0] = 0;
			pc++;
			break;
		}
		//cache miss
		else {

		}
	}
	case 17: //sw opcode
	{
		if (tsram[get_index(regs[cmd.rd])]->msi == 0) {//invalid need to get data from main memory
			stat->read_miss += 1;
			if (last_bus->bus_busy == 1) {

				//put_stall() need to remember to count the stalls.
				return pc;
			}
			else {
				last_bus->bus_origid = pipe->core_id;
				last_bus->bus_busy = 1;//start transaction
				last_bus->bus_cmd = 1;//bus read
				last_bus->flush_cycle = cycle + 64;
				last_bus->bus_addr = regs[cmd.rs] + regs[cmd.rt];
			}
			pc++;
			break;
		}
		else {// i have the data in my dsram so i can get it
			lw(regs, cmd, dsram);
			regs[0] = 0;
			pc++;
			break;
		}

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
void mul(int* regs, Command cmd)
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
void lw(int* regs, Command cmd, unsigned int* dsram)
{
	if (regs[cmd.rs] + regs[cmd.rt] < DSRAM_SIZE)
		regs[cmd.rd] = dsram[get_index(regs[cmd.rs] + regs[cmd.rt])];
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

