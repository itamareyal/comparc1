/*
core.h
*/


/*------------------------------------------------------------------------------------
										INCLUDE
------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------
										DEFINES
------------------------------------------------------------------------------------*/
#define I_MEM_SIZE 1024
#define DSRAM_SIZE 256
#define TSRAM_SIZE 256
#define MAIN_MEM_SIZE 1048576 // check for updates

typedef struct _bus {
	unsigned int bus_origid; //3 bits. 0-3 cores; 4 mem
	unsigned int bus_cmd;	 //2 bits
	unsigned int bus_addr;	 //20 bits of address
	unsigned int bus_data;	 //32 bits of data. 1 word
}Bus;

typedef struct _tsram {
	unsigned int msi;		 //2 bits 0-i 1-s 2-m
	unsigned int tag;		 //12 bits
}TSRAM;

typedef struct _pipe {
	int core_id;
	Command IF;
	Command ID;
	Command EX;
	Command MEM;
	Command WB;
}PIPE;

/*------------------------------------------------------------------------------------
										DECLARATION
------------------------------------------------------------------------------------*/

