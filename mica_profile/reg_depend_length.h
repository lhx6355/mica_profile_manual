//
// This is to calculate register dependence distance
// instReg: InstReg struct that includes destation
// register and Source register
// instNum: the number of latest instruction
//
// Created by Xu Wenjuan
//

#ifndef REGDEPENDLENGTH_H
#define REGDEPENDLENGTH_H

#include "global_def.h"
#include "inst_locality.h"

struct RegInfo{
	int srcInstNum = 0;		// [lhx] 该寄存器最近一次作为源寄存器的指令编号
	int desInstNum = 0;		// [lhx] 该寄存器最近一次作为目的寄存器的指令编号
};

#define REG_DEP_D_SIZE	30

class RegDepend {
	Trace trace;
	RegInfo regInfo[REG_NUM];// '*2' added by WenjFu 11/07 
	RegInfo regInfoFp[REG_NUM];// first 32 is A32 regs and last 32 is A64 regs
	//int *regDepLen;
	/* for debug */
	int regDepLen[REG_DEP_D_SIZE];
	int distSize; /* distribution array size */

public:
	/* set distribution array size , default is 20 */
	RegDepend(const int & size) : distSize(size) {
		//regDepLen = new int[distSize];
		for (int i = 0; i < distSize; i++)
			regDepLen[i] = 0;
	}

	RegDepend() : RegDepend(REG_DEP_D_SIZE) {};

	~RegDepend() {
		//delete[] regDepLen;
	}

	void regDependLength(InstReg &instReg, InstType &instType, const uint8_t &instNum);

	const Trace & getTrace() const { return trace; }

	const int * getRegDepLen() const { return regDepLen; }

	const int & getSize() const { return distSize; }
};

#endif
