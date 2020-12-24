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
	PIPE_ptr pipe, FILE* fp_trace, BUS_ptr last_bus, unsigned int* dsram, TSRAM_ptr tsram[]) {
	if (pc == -1)
		return -1;
	int inst = imem[pc];
	Command cmd = line_to_command(inst, core_id); // create Command struct
	update_pipeline(pipe, pc);
	char line_for_trace[MAX_LINE_TRACE] = { 0 }; //create line for trace file
	create_line_for_trace(line_for_trace, regs, pc, cycle, pipe);//append to trace file
	fprintf_s(fp_trace, "%s\n", line_for_trace);
	regs[1] = sign_extend(cmd.immiediate);//first we do sign extend to immiediate
	//snoop_bus(last_bus, tsram);
	//check_flush(last_bus, tsram, imem);
	pc = execution(regs, pc, cmd, imem);
	return pc;
}

void snoop_bus(BUS_ptr last_bus, TSRAM_ptr tsram[], int* cycle) {
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
			if (last_bus->bus_busy == 0) {
				last_bus->bus_busy = 1;
				last_bus->flush_cycle = *cycle + 64;
				tsram[index]->msi = 1;
			}
			break;
		}
		}
	case 2 : // BusRdX
	{
		if (tsram[index]->msi == 0 || tsram[index]->msi == NULL)
			break;
		else if (tsram[index]->msi == 1) {
			tsram[index]->msi == 0;
			break;
		}
		else if (tsram[index]->msi == 2) {
			if (last_bus->bus_busy == 0) {
				last_bus->bus_busy = 1;
				last_bus->flush_cycle = cycle + 64;
				tsram[index]->msi = 1;
			}
			break;
		}
	}
	case 3: // Flush
	{
		break;
	}
	}
}

void execution_bus(BUS_ptr last_bus, TSRAM_ptr tsram) {
	switch (last_bus->bus_cmd) {
	case 0: // no cmd
	{
		break;
	}
	case 1: // BusRd
	{

	}
	case 2: // BusRdX
	{

	}
	case 3: // Flush
	{

	}
	}
}

void check_flush(BUS_ptr last_bus, TSRAM_ptr tsram, unsigned int mem[], int* bus_busy,int* cycle,int* flush_cycle) {
	if (*bus_busy == 1) {
		if (flush_cycle == cycle)
			;
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
	Bus->data_owner = 0;
	Bus->flush_cycle = -1;//intilize to these value because we do cycle==flush cycle from start.
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
	
void create_line_for_trace(char line_for_trace[], int regs[], int pc, unsigned int cycle, PIPE_ptr pipe)
{
	//initinlize parameters
	int i;
	char cycle_char[MAX_PC_CHAR] = { 0 };
	char temp_reg_char[BUFFER_MAX_SIZE] = { 0 };
	char* stall = "---";

	//add cycle and fetch to the output line
	sprintf_s(cycle_char, MAX_PC_CHAR, "%d", cycle);
	sprintf_s(line_for_trace, BUFFER_MAX_SIZE, cycle_char);
	sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");

	//handle pipeline. add each value manuelly
	if (pipe->IF == STALL) {
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, stall);//add to line
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");
	}
	else {
		sprintf_s(temp_reg_char, BUFFER_MAX_SIZE, "%d", pipe->IF);
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, temp_reg_char);//add to line
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");
	}
	if (pipe->ID == STALL) {
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, stall);//add to line
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");
	}
	else {
		sprintf_s(temp_reg_char, BUFFER_MAX_SIZE, "%d", pipe->ID);
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, temp_reg_char);//add to line
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");
	}
	if (pipe->EX == STALL) {
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, stall);//add to line
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");
	}
	else {
		sprintf_s(temp_reg_char, BUFFER_MAX_SIZE, "%d", pipe->EX);
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, temp_reg_char);//add to line
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");
	}
	if (pipe->MEM == STALL) {
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, stall);//add to line
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");
	}
	else {
		sprintf_s(temp_reg_char, BUFFER_MAX_SIZE, "%d", pipe->MEM);
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, temp_reg_char);//add to line
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");
	}
	if (pipe->WB == STALL) {
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, stall);//add to line
		sprintf_s(line_for_trace + strlen(line_for_trace), BUFFER_MAX_SIZE, " ");
	}
	else {
		sprintf_s(temp_reg_char, BUFFER_MAX_SIZE, "%d", pipe->WB);
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


// ask for bus function and execute