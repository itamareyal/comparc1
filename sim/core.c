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

int core_execution(int* cycle, int pc, int core_id, unsigned int *imem, int *regs,
	PIPE_ptr pipe, FILE* fp_trace, BUS_ptr last_bus, unsigned int* dsram,
	TSRAM tsram[],STAT_ptr stat, Watch_ptr watch) {
	//if we are in halt dont come in the function at all
	if (pc == -1)
		return -1;

	int inst, branch_resolution=-1;
	Command cmd_if, cmd_id, cmd_exe, cmd_mem, cmd_wb;
	int increment_pc = 1; //by defaulf we inc pc by 1

	//fetch only if we are not in stall
	if (pc >= -2) {
		inst = imem[pc];
		cmd_if = line_to_command(inst, core_id); // create Command struct
	}
	update_pipeline(pipe, pc);
	char line_for_trace[MAX_LINE_TRACE] = { 0 }; //create line for trace file
	create_line_for_trace(line_for_trace, regs, pc, cycle, pipe);//append to trace file
	fprintf_s(fp_trace, "%s\n", line_for_trace);
	snoop_bus(last_bus, tsram,cycle,core_id, dsram);

	//decode to see errors
	inst = imem[pipe->ID];
	cmd_id = line_to_command(inst, core_id);
	regs[1] = sign_extend(cmd_id.immiediate);
	inst = imem[pipe->EX];
	cmd_exe = line_to_command(inst, core_id);
	inst = imem[pipe->MEM];
	cmd_mem = line_to_command(inst, core_id);
	inst = imem[pipe->WB];
	cmd_wb = line_to_command(inst, core_id);
	if (data_hazard(cmd_id, cmd_exe, cmd_mem, cmd_wb)) {//detect data hazrds
		pipe->WB = pipe->MEM;
		pipe->MEM = pipe->EX;
		pipe->EX = -10;
		increment_pc = 0;
	}
	if (cmd_id.opcode == 20 || cmd_exe.opcode == 20 || cmd_mem.opcode == 20|| cmd_wb.opcode==20) {//we handle halt in decode stage
		pipe->IF = -10;
		branch_resolution = -10;
	}

	//branch resolution of decode
	if (cmd_id.opcode <= 15 && cmd_id.opcode >= 9) {//jal=> have to jump, other jump opcode maybe
		branch_resolution = execution(regs, pc, cmd_id, imem, last_bus, dsram, tsram, stat, pipe, cycle, watch);
	}

	//memory
	//index and tag for the memory store and load
	int index = get_index(regs[cmd_mem.rs] + regs[cmd_mem.rt]);
	int tag = get_tag(regs[cmd_mem.rs] + regs[cmd_mem.rt]);

	//for lw and ll opcode in the memory stage
	if (cmd_mem.opcode == 16 || cmd_mem.opcode == 18) {//if we are in lw or ll
		//cache hit
		if (tsram[index].tag == tag && tsram[index].msi != 0) {// i have the data in my dsram so i can get it
			stat->read_hit += 1;
			//pc++;
		}
		//invalid data (cache miss)
		else if (tsram[index].msi == 0 || tsram[index].msi == 1) {
			stat->read_miss += 1;
			if (last_bus->bus_busy == 1) {//put stall
				stat->mem_stall += 1;
				pipe->WB = -10;
			}
			else {
				if (last_bus->bus_origid != pipe->core_id) {//create bus read
					last_bus->bus_origid = pipe->core_id;
					last_bus->bus_busy = 1;//start transaction
					last_bus->bus_cmd = 1;//bus read
					last_bus->flush_cycle = cycle + 64;
					last_bus->bus_addr = regs[cmd_mem.rs] + regs[cmd_mem.rt];
					stat->read_hit -= 1;//becuase in the lw its not really hit
				}
				stat->mem_stall += 1;
				pipe->WB = -10;
			}
			increment_pc = 0;
		}
		//cache miss incorrect tag
		else {
			if (tsram[index].msi == 2) {//if the data in cache is modified we need to flush it to main memory
				if (last_bus->bus_busy == 1) {
					stat->mem_stall += 1;
					pipe->WB = -10;
				}
				else {
					tsram[index].msi = 1;
					last_bus->bus_origid = pipe->core_id;
					last_bus->bus_busy = 1;//start transaction
					last_bus->bus_cmd = 3;//bus flush
					last_bus->flush_cycle = -1;
					last_bus->bus_addr = regs[cmd_mem.rs] + regs[cmd_mem.rt];
					last_bus->bus_data = dsram[index];
					stat->mem_stall += 1;
					pipe->WB = -10;
				}
			}
			increment_pc = 0;
		}
	}
	//sw and sc opcodes handle in memory stage
	if (cmd_mem.opcode == 17 || cmd_mem.opcode == 19) {
		if (tsram[index].msi == 2 && tsram[index].tag == tag) {//write hit
			stat->write_hit += 1;
			//pc++;
		}
		else if (tsram[index].msi == 2 && tsram[index].tag != tag) {//modified need to  flush data to main memory
			stat->write_miss += 1;
			if (last_bus->bus_busy == 1) {
				stat->mem_stall += 1;
				pipe->WB = -10;
			}
			else {
				tsram[index].msi = 1;
				last_bus->bus_origid = pipe->core_id;
				last_bus->bus_busy = 1;//start transaction
				last_bus->bus_cmd = 3;//bus flush
				last_bus->data_destination = 4;
				last_bus->flush_cycle = -1;
				last_bus->bus_addr = regs[cmd_mem.rs] + regs[cmd_mem.rt];
				last_bus->bus_data = dsram[index];
			}
			increment_pc = 0;
		}
		else {//shared or invalid modes we have to do busrdx
			if (last_bus->bus_busy == 1) {
				stat->mem_stall += 1;
				pipe->WB = -10;
			}
			else {
				stat->write_miss += 1;
				last_bus->bus_origid = pipe->core_id;
				last_bus->bus_busy = 1;//start transaction
				last_bus->bus_cmd = 2;//busrdx
				last_bus->data_destination = pipe->core_id;
				last_bus->flush_cycle = cycle + 64;
				last_bus->bus_addr = regs[cmd_mem.rs] + regs[cmd_mem.rt];
			}
		}
		increment_pc = 0;
	}
	
	//write back
	if (cmd_wb.opcode > 15 || cmd_wb.opcode < 9) {//cant execute the jump opcode again
		regs[1] = sign_extend(cmd_wb.immiediate);//first we do sign extend to immiediate
		pc = execution(regs, pc, cmd_wb, imem, last_bus, dsram, tsram, stat, pipe, cycle, watch);
		if (pc == -1)
			return -1;
	}

	if (branch_resolution != -1 && (branch_resolution!=pc || branch_resolution==-10)) // update new pc to a branch taken
		return branch_resolution;
	else // update new pc to the same or next pc
		return pc + increment_pc;

}

int data_hazard(Command id, Command exe, Command mem, Command wb) {
	//1 for data hazard and 0 for clear
	if ((exe.opcode>-1 &&exe.opcode < 9) || exe.opcode==16 || exe.opcode==18) // arithmetic cmd saving new data to rd
	{
		if (exe.rd == id.rt || exe.rd == id.rs)
			return 1;
	}
	
	if ((mem.opcode > -1 && mem.opcode < 9) || mem.opcode == 16 || mem.opcode == 18) // arithmetic cmd saving new data to rd
	{
		if (mem.rd == id.rt || mem.rd == id.rs)
			return 1;
	}

	if ((wb.opcode > -1 && wb.opcode < 9) || wb.opcode == 16 || wb.opcode == 18) // arithmetic cmd saving new data to rd
	{
		if (wb.rd == id.rt || wb.rd == id.rs)
			return 1;
	}
	return 0;
}

int compare_bus(BUS_ptr prev_bus, BUS_ptr curr_bus) {
	if (prev_bus->bus_origid != curr_bus->bus_origid)
		return 0;
	if (prev_bus->bus_cmd != curr_bus->bus_cmd)
		return 0;
	if (prev_bus->bus_addr != curr_bus->bus_addr)
		return 0;
	if (prev_bus->bus_data != curr_bus->bus_data)
		return 0;
	return 1;
}

void copy_bus(BUS_ptr prev_bus, BUS_ptr curr_bus) {
	prev_bus->bus_origid = curr_bus->bus_origid;
	prev_bus->bus_cmd = curr_bus->bus_cmd;
	prev_bus->bus_addr = curr_bus->bus_addr;
	prev_bus->bus_data = curr_bus->bus_data;
}

void snoop_bus(BUS_ptr last_bus, TSRAM tsram[], int* cycle, int core_id, unsigned int* dsram) {
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
		if (tsram[index].msi == 0 || tsram[index].msi == NULL)
			break;
		else if (tsram[index].msi == 1)
			break;
		else if (tsram[index].msi == 2) {
			last_bus->bus_data = dsram[get_index(last_bus->bus_addr)];
			last_bus->data_owner = core_id;
			tsram[index].msi = 1;
			break;
		}
		}
	case 2 : // BusRdX
	{
		if (tsram[index].msi == 0 || tsram[index].msi == NULL)
			break;
		else if (tsram[index].msi == 1) {
			tsram[index].msi = 0;
			break;
		}
		else if (tsram[index].msi == 2) {
			last_bus->bus_data = dsram[index];
			last_bus->data_owner = core_id;
			tsram[index].msi = 0;
			break;
		}
	}
	case 3: // Flush
	{
		if (core_id == last_bus->data_destination) {
			if (last_bus->prev_cmd == 2)
				tsram[index].msi = 2;
			else
				tsram[index].msi = 1;
			tsram[index].tag = tag;
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
			else if (cycle == last_bus->flush_cycle) {//made flush
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
			mem[last_bus->bus_addr] = last_bus->bus_data;
			// Flush had happened, reset bus to idle
			last_bus = initilize_bus();
		}
		}
	}
}

void init_pipe(int core_id, PIPE_ptr pipe) 
{
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
	if (pc == pipe->IF && pc>=0)
		return;
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
	for (i = 2; i < 15; i++) {
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

void initilize_watch(Watch_ptr watch) {
	int i = 0;
	for (i = 0; i < 4; i++) {
		watch->address[i] = -1;
		watch->lock[i] = 0;
	}
}

void create_line_for_bus(char line_for_bus[], int cycle, BUS_ptr last_bus) {
	int i;
	char cycle_char[MAX_PC_CHAR] = { 0 };
	char temp_char[MAX_PC_CHAR] = { 0 };

	//add cycle and fetch to the output line
	sprintf_s(cycle_char, MAX_PC_CHAR, "%d", cycle / 4);
	sprintf_s(line_for_bus, BUFFER_MAX_SIZE, cycle_char);
	sprintf_s(line_for_bus + strlen(line_for_bus), BUFFER_MAX_SIZE, " ");

	//handle all the bus parameters
	sprintf_s(temp_char, MAX_PC_CHAR, "%01X", last_bus->bus_origid);
	sprintf_s(line_for_bus + strlen(line_for_bus), BUFFER_MAX_SIZE, temp_char);//add to line
	sprintf_s(line_for_bus + strlen(line_for_bus), BUFFER_MAX_SIZE, " ");

	sprintf_s(temp_char, MAX_PC_CHAR, "%01X", last_bus->bus_cmd);
	sprintf_s(line_for_bus + strlen(line_for_bus), BUFFER_MAX_SIZE, temp_char);//add to line
	sprintf_s(line_for_bus + strlen(line_for_bus), BUFFER_MAX_SIZE, " ");

	sprintf_s(temp_char, MAX_PC_CHAR, "%05X", last_bus->bus_addr);
	sprintf_s(line_for_bus + strlen(line_for_bus), BUFFER_MAX_SIZE, temp_char);//add to line
	sprintf_s(line_for_bus + strlen(line_for_bus), BUFFER_MAX_SIZE, " ");

	sprintf_s(temp_char, MAX_PC_CHAR, "%.8X", last_bus->bus_data);
	sprintf_s(line_for_bus + strlen(line_for_bus), BUFFER_MAX_SIZE, temp_char);//add to line
}

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

// this function creates a struct Command from a string in memory
Command line_to_command(unsigned int inst, int core_id)
{
	Command cmd;
	if (inst == -10) {// if the command is stall we put stall
		cmd.opcode = -10;
		cmd.rd = -10;
		cmd.rs = -10;
		cmd.rt = -10;
		cmd.immiediate = -10;
		cmd.core_id = core_id;
		return cmd;
	}
	cmd.opcode = (get_byte(inst, 7) * 16) + get_byte(inst, 6);
	cmd.rd = get_byte(inst, 5);
	cmd.rs = get_byte(inst, 4);
	cmd.rt = get_byte(inst, 3);
	cmd.immiediate = (get_byte(inst, 2) * 16 * 16) + (get_byte(inst, 1) * 16) + get_byte(inst, 0);
	cmd.core_id = core_id;
	return cmd;
}


int execution(int regs[], int pc, Command cmd, unsigned int* mem, BUS_ptr last_bus, unsigned int* dsram,
	TSRAM tsram[], STAT_ptr stat, PIPE_ptr pipe,int* cycle,Watch_ptr watch) {

	//index and tag for the memory store and load
	int index = get_index(regs[cmd.rs] + regs[cmd.rt]);
	int tag = get_tag(regs[cmd.rs] + regs[cmd.rt]);

	switch (cmd.opcode)
	{
	case 0: //add opcode
	{
		if (cmd.rd == 0 && cmd.rs == 0 && cmd.rt == 0 && cmd.immiediate == 0)
			break; // it's a stall
		else {
			add(regs, cmd);
			regs[0] = 0; // make sure $zero is zero
			break;
		}
	}
	case 1: //sub opcode
	{
		sub(regs, cmd);
		regs[0] = 0;
		break;
	}
	case 2: //and opcode
	{
		and (regs, cmd);
		regs[0] = 0;
		break;
	}
	case 3://or opcode
	{
		or (regs, cmd);
		regs[0] = 0;
		break;
	}
	case 4: //xor opcode
	{
		xor (regs, cmd);
		regs[0] = 0;
		break;
	}
	case 5: //mul opcode
	{
		mul(regs, cmd);
		regs[0] = 0;
		break;
	}
	case 6: //sll opcode
	{
		sll(regs, cmd);
		regs[0] = 0;
		break;
	}
	case 7: //sra opcode
	{
		sra(regs, cmd);
		regs[0] = 0;
		break;
	}
	case 8: //srl opcode
	{
		srl(regs, cmd);
		regs[0] = 0;
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
		lw(regs, cmd, dsram);
		break;
	}
	case 17: //sw opcode
	{
		sw(regs, cmd, dsram, tsram);
		break;
	}
	case 18: //ll opcode
	{
		ll(regs, cmd, dsram,watch,pipe->core_id);
		regs[0] = 0;
		break;
	}
	case 19: //sc opcode
	{
		sc(regs, cmd, dsram,watch);
		regs[0] = 0;
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
		//pc++;
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
	if (get_index(regs[cmd.rs] + regs[cmd.rt]) < DSRAM_SIZE)
		regs[cmd.rd] = dsram[get_index(regs[cmd.rs] + regs[cmd.rt])];
}

//sw command.
void sw(int* regs, Command cmd, unsigned int* dsram, TSRAM tsram[])
{
	if (get_index(regs[cmd.rs] + regs[cmd.rt]) < DSRAM_SIZE) {
		dsram[get_index(regs[cmd.rs] + regs[cmd.rt])] = regs[cmd.rd];
		tsram[get_index(regs[cmd.rs] + regs[cmd.rt])].msi = 2;
		tsram[get_index(regs[cmd.rs] + regs[cmd.rt])].tag = get_tag(regs[cmd.rs] + regs[cmd.rt]);
	}
}

//ll command.
void ll(int* regs, Command cmd, unsigned int* dsram,Watch_ptr watch, int core_id)
{
	if (get_index(regs[cmd.rs] + regs[cmd.rt]) < DSRAM_SIZE) {
		regs[cmd.rd] = dsram[get_index(regs[cmd.rs] + regs[cmd.rt])];
		watch->lock[core_id] = 1;
		watch->address[core_id] = regs[cmd.rs] + regs[cmd.rt];
	}
}

//sw command.
void sc(int* regs, Command cmd, unsigned int* dsram , Watch_ptr watch)
{

}

