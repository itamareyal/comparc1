/*
core.c

Hold functions to dispatch and manage cores in the program
*/


/*------------------------------------------------------------------------------------
										INCLUDE
------------------------------------------------------------------------------------*/
#include <stdio.h>
#include "core.h"
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
// core loop
void core_execution(int pc, int core_id, int inst, unsigned int *imem, int *regs, FILE* fp_trace) {
	if (pc <= I_MEM_SIZE - 1)
		inst = imem[pc];
	else
	{
		pc = -1;
	}
	Command cmd = line_to_command(inst); // create Command struct

	char line_for_trace[200] = { 0 }; //create line for trace file
	create_line_for_trace(line_for_trace, regs, pc, inst, cmd.immiediate);//append to trace file
	fprintf(fp_trace, "%s\n", line_for_trace);
	regs[1] = sign_extend(cmd.immiediate);//first we do sign extend to immiediate

	//pc = execution(regs, pc, cmd);

}



// snoop function- update tsram
void snoop_bus(Bus last_bus, TSRAM ts) {
	switch (last_bus.bus_cmd) {
	case 0: // no cmd
	{
	}
	case 1: // BusRd
	{
	}
	case 2 : // BusRdX
	{

	}
	case 3: // Flush
	{
	}
	}
}


// ask for bus function and execute