/*
core.c

Hold functions to dispatch and manage cores in the program
*/


/*------------------------------------------------------------------------------------
										INCLUDE
------------------------------------------------------------------------------------*/
#include <stdio.h>
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
void manage_cores(int cycle, int pc, int core_id, int inst, unsigned int* imem, int* regs, FILE* fp_trace) {
	/*int pc_0, pc_1, pc_2, pc_3 = 0;
	core_execution(pc_0,)*/

}

// core loop
int core_execution(int cycle, int pc, int core_id, unsigned int *imem, int *regs,PIPE_ptr pipe, FILE* fp_trace) {
	int inst = imem[pc];
	Command cmd = line_to_command(inst, core_id); // create Command struct
	update_pipeline(pipe, pc);
	char line_for_trace[MAX_LINE_TRACE] = { 0 }; //create line for trace file
	create_line_for_trace(line_for_trace, regs, pc, cycle,pipe);//append to trace file
	fprintf(fp_trace, "%s\n", line_for_trace);
	regs[1] = sign_extend(cmd.immiediate);//first we do sign extend to immiediate
	pc = execution(regs, pc, cmd,imem);
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
//initilize the pipeline for the rest of the program
PIPE_ptr init_pipe(int core_id) 
{
	PIPE pipe;
	pipe.core_id = core_id;
	pipe.IF = '---';
	pipe.ID = '---';
	pipe.EX = '---';
	pipe.MEM = '---';
	pipe.WB = '---';
}

void  initilize_pipelines(PIPE_ptr pipe_0, PIPE_ptr pipe_1, PIPE_ptr pipe_2, PIPE_ptr pipe_3)
{
	pipe_0 = init_pipe(0);
	pipe_1 = init_pipe(1);
	pipe_2 = init_pipe(2);
	pipe_3 = init_pipe(3);
}

void update_pipeline(PIPE_ptr pipe, int pc)
{
	pipe->WB = pipe->MEM;
	pipe->MEM = pipe->EX;
	pipe->EX = pipe->ID;
	pipe->ID = pipe->IF;
	pipe->IF = pc;
}
	


// ask for bus function and execute