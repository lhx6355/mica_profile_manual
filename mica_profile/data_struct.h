#ifndef DATA_STRUCT
#define DATA_STRUCT

#include <stdint.h>

#define	TRACE_BUFFER_SIZE	10000000
#define TRACE_INST_START	1+133

#define	DEFAULT_CALLBACK_STATE	1	// 1: start trace

// dummy1 stores inst_mode frome 0x00 to 0x1f
#define	DUMMY1_LOAD_FLAG	0x20 
#define	DUMMY1_STORE_FLAG	0x40

#define	CPU_COMPONENT		184

#define FILENAME_SIZE		128
//#define FILENAME_PATH		"Z:\\WenjFu_share\\HW step5\\trace_data\\"

struct InstTrace
{
	uint32_t	opcode = 0;
	uint8_t		inst_iset;
	uint8_t		dummy0;
	uint8_t		dummy1;
	uint8_t		dummy2;
	uint64_t	pc = 0;
	uint64_t	inst_paddr = 0;
	uint64_t	load_paddr = 0;
	uint64_t	load_vaddr = 0;
	uint64_t	store_paddr = 0;
	uint64_t	store_vaddr = 0;
};

#endif // DATA_STRUCT
