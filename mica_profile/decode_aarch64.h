//
// Created by shenshan on 16-9-22.
//

#ifndef DECODE_AARCH64_H
#define DECODE_AARCH64_H
#include "global_def.h"

namespace aarch64 {
	Aarch64Set decodeA64(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch64Set decodeDataProcImm(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch64Set decodeBranchExcSys(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch64Set decodeLoadsStores(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch64Set decodeDataProcReg(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch64Set decodeFpAdvSIMD(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch64Set decodeFp(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch64Set decodeAdvSIMD(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch64Set decodeAdvSIMDScalar(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	// AdvSIMD three same
	inline Aarch64Set decodeNeon3Same(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg);
	// AdvSIMD three different
	inline Aarch64Set decodeNeon3Diff(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg);
	// AdvSIMD two-reg misc
	inline Aarch64Set decodeNeon2RegMisc(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg);
	// AdvSIMD across lanes
	inline Aarch64Set decodeNeonAcrossLanes(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg);
	// AdvSIMD copy
	inline Aarch64Set decodeNeonCopy(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg);
	// AdvSIMD vector x indexed element
	inline Aarch64Set decodeNeonIndexedElem(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg);
	// AdvSIMD modified immediate
	inline Aarch64Set decodeNeonModImm(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg);
	// AdvSIMD shift by immediate
	inline Aarch64Set decodeNeonShiftByImm(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg);
	// AdvSIMD TBL/TBX
	inline Aarch64Set decodeNeonTblTbx(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg);
	// AdvSIMD ZIP/UZP/TRN
	inline Aarch64Set decodeNeonZipUzpTrn(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg);
	// AdvSIMD EXT
	inline Aarch64Set decodeNeonExt(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg);
	// AdvSIMD scalar three same
	inline Aarch64Set decodeNeonSc3Same(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg);
	// AdvSIMD scalar three different
	inline Aarch64Set decodeNeonSc3Diff(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg);
	// AdvSIMD scalar two-reg misc
	inline Aarch64Set decodeNeonSc2RegMisc(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg);
	// AdvSIMD scalar pairwise
	inline Aarch64Set decodeNeonScPwise(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg);
	// AdvSIMD scalar copy
	inline Aarch64Set decodeNeonScCopy(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg);
	// AdvSIMD scalar x indexed element
	inline Aarch64Set decodeNeonScIndexedElem(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg);
	// AdvSIMD scalar shift by immediate
	inline Aarch64Set decodeNeonScShiftByImm(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg);
	// AdvSIMD load/store
	inline Aarch64Set decodeNeonMem(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg);
}

#endif //DECODE_AARCH64_H
