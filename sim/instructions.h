/*
instructions.h
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
	unsigned int pipeline;
	unsigned int stall_flag;
	unsigned int core_id;
}Command;


/*------------------------------------------------------------------------------------
										DECLARATION
------------------------------------------------------------------------------------*/
Command line_to_command(unsigned int inst);
