//
// This is to calculate branch behaviors
// inst: the latest Inst struct
// nextInst: the next Inst struct
// instType: type of the opcode
// branch: stores branch behaviors
//
// Created by Fu Wenjie
//
#include "global_def.h"
#include "branch_behavior.h"

#define DIST_SCALE	100

void BranchBehavior::calBranchBehavior(const InstTrace &inst, const InstTrace &nextInst, const bool &isBranch)
{
	static int preState = NINF;  /* ensure won't compare with state at the first time */
	static bool first = false;

	/* for the first branch */
	if (!isBranch && !first)
		return;
	else if (isBranch) {

		//[modified by pz] 并非所有指令长度均为32位
		if (inst.inst_iset == ISET_THUMB && (inst.opcode >> 16) == 0)
			instLength = 2;
		else instLength = 4;

		uint8_t state = 0;
		/* jump forward if next pc minus 4 is lower than now */
		if (inst.pc > (nextInst.pc - instLength))
		{
			++branch.fwBranchs;
			state = 1;					// [LHX] 跳了

			uint64_t branchJmpDis = minus64(inst.inst_paddr, nextInst.inst_paddr) / DIST_SCALE;
			/* uint64 to int */
			if ((int)branchJmpDis < 0)
				branchJmpDis = 0;

			if (branchJmpDis > distSize - 1) 
				branchJmpDis = distSize - 1;
			/* distance distribution */
			++branchJumpDistance[branchJmpDis];
		}
		/* whether calculating jump distance in this situation?? */
		else if (inst.pc == (nextInst.pc - instLength))
		{
			++branch.notTakenBranchs;
			state = 0;
		}
		else if (inst.pc < (nextInst.pc - instLength))
		{
			++branch.bwBranchs;
			state = 1;
																									/* calculate the distance, but why physical address? */
			uint64_t branchJmpDis = minus64(nextInst.inst_paddr, inst.inst_paddr) / DIST_SCALE;
			/* uint64 to int */
			if ((int)branchJmpDis < 0)
				branchJmpDis = 0;

			if (branchJmpDis > distSize - 1)
				branchJmpDis = distSize - 1;
			/* distance distribution */
			++branchJumpDistance[branchJmpDis];
		}

		if (preState != NINF)
			if (preState != state)
				++branch.takenChanged;
		
		/* update static preState */
		preState = state;	
	}

	if (!isBranch)
		/* number of instructions between branches */
		++branch.CtrlNum;
	/* calculate distance between two branches*/
	else if (isBranch && first) {
		uint32_t blockSize = 0;
		while (branch.CtrlNum)
		{
			++blockSize;
			branch.CtrlNum >>= 1;
		}
		/* calc basicblocksize distribution */
		if (blockSize > distSize - 1) 
			blockSize = distSize - 1;
		++branchBasicBlockSize[blockSize];
	}
	/* first encounter a branch */
	else 
		first = true;
}
