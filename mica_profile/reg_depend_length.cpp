//
// This is to calculate register dependence distance
// instReg: InstReg struct that includes destation
// register and Source register
// instNum: the number of latest instruction
//
// Created by Xu Wenjuan
//

#include "global_def.h"
#include "reg_depend_length.h"


void RegDepend::regDependLength(InstReg &instReg, InstType &instType, const uint8_t &instNum)
{
	if (instType.isUnKnown)
		return;

	for (int i = 0; i < SREG_SIZE; i++){
		if (instReg.isSrcLegel(i)) {
			if (instReg.isFpSrcReg(i)) {
				/* this will be a source register, increase read reg times */
				/* if has source registers, justify the reg depend distance */
				if (0 != regInfoFp[instReg.SReg[i]].desInstNum) {
					int pathLength;
					regInfoFp[instReg.SReg[i]].srcInstNum = instNum;
					pathLength = regInfoFp[instReg.SReg[i]].srcInstNum - regInfoFp[instReg.SReg[i]].desInstNum - 1; // add -1 by WenjFu 11/07

					if ((pathLength < distSize) && (pathLength >= 0))
						++regDepLen[pathLength];
				}
			}
			else {
				if (0 != regInfo[instReg.SReg[i]].desInstNum) {
					int pathLength;
					regInfo[instReg.SReg[i]].srcInstNum = instNum;
					pathLength = regInfo[instReg.SReg[i]].srcInstNum - regInfo[instReg.SReg[i]].desInstNum - 1; // add -1 by WenjFu 11/07
					/* this will be a source register, increase read reg times */
					if ((pathLength < distSize) && (pathLength >= 0))
						regDepLen[pathLength]++;
				}
			}
		}
	}

	/* if has a destation register, store the index and increase Write times */
	for (int i = 0; i < DREG_SIZE; ++i) {
		if (instReg.isDesLegel(i)) {
			if (instReg.isFpDesReg(i))
				regInfoFp[instReg.DReg[i]].desInstNum = instNum;
			else
				regInfo[instReg.DReg[i]].desInstNum = instNum;
		}
	}

	// by shen 11/13
	if (instType.isCc)
		/* conditional inst set PSTATE register */
		++trace.ccRegWriteNum;

	//	for (int i = 0; i < SREG_SIZE; i++)
	//		if (instReg.isSrcLegel(i))
	//			++trace.ccRegReadNum;
	//}
	//if (instType.isMISC){
	//	/* if has a destation register, store the index and increase Write times */
	//	for (int i = 0; i < DREG_SIZE; ++i)
	//		if (instReg.isDesLegel(i))
	//			++trace.miscRegWriteNum;

	//	for (int i = 0; i < SREG_SIZE; i++){
	//		if (instReg.isSrcLegel(i))
	//			++trace.miscRegReadNum;
	//	}
	//}

	/* if has a destation register, store the index and increase Write times */
	for (int i = 0; i < DREG_SIZE; ++i) {
		if (instReg.isDesLegel(i)) {
			if (instReg.isFpDesReg(i))
				++trace.fpRegWriteNum;
			else
				++trace.intRegWriteNum;
		}
	}

	for (int i = 0; i < SREG_SIZE; ++i){
		if (instReg.isSrcLegel(i)) {
			if (instReg.isFpSrcReg(i))
				++trace.fpRegReadNum;
			else
				++trace.intRegReadNum;
		}
	}
}