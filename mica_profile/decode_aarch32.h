//
// Created by WenjFu on 16-11-04.
//

#ifndef DECODE_AARCH32_H
#define DECODE_AARCH32_H
#include "global_def.h"
#include "decode_aarch64.h"

#define THUMB          bits(machInst, 36)
#define ENCODING       bits(machInst, 27, 25)
#define COND_CODE      bits(machInst, 31,28)
#define SEVEN_AND_FOUR (bits(machInst, 7) && bits(machInst, 4))
#define PREPOST		   bits(machInst, 24)
#define MISC_OPCODE    bits(machInst, 7, 4)
#define IS_MISC        (bits(machInst, 24, 23) == 0x2 &&  bits(machInst, 20) == 0)
#define OPCODE_7       bits(machInst, 7)
#define OPCODE         bits(machInst, 24, 21)
#define OPCODE_22      bits(machInst, 22)
#define PUBWL          bits(machInst, 24, 20)
#define OPCODE_4       bits(machInst, 4)
#define OPCODE_24_23   bits(machInst, 24, 23)
#define OPCODE_24      bits(machInst, 24)
#define CPNUM          bits(machInst, 11, 8)
#define MEDIA_OPCODE   bits(machInst, 24, 20)
#define OPC2           bits(machInst, 7,5)
#define OPCODE_20      bits(machInst, 20)
#define IMMED_HI_11_8  bits(machInst, 11, 8)
#define IMMED_LO_3_0   bits(machInst, 3, 0)
#define IMM   bits(machInst, 7, 0)
#define RN bits(machInst, 19, 16)
#define RD bits(machInst, 15, 12)
#define RT bits(machInst, 15, 12)
#define RM bits(machInst, 3, 0)
#define RS bits(machInst, 11, 8)

namespace aarch32 {
	Aarch32Set decodeT32(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	Aarch32Set decodeA32(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set decodeNeonMem(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set Thumb16ShiftAddSubMoveCmp(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set Thumb16DataProcessing(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set Thumb16SpecDataAndBx(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set Thumb16MemLit(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set Thumb16MemReg(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set Thumb16MemImm(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set Thumb16Adr(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set Thumb16AddSp(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set Thumb16Misc(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set Thumb16MacroMem(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set Thumb16CondBranchAndSvc(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set Thumb16UncondBranch(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set Thumb32SrsRfe(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set Thumb32LdrStrDExTbh(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set Thumb32DataProcShiftReg(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set Thumb32DataProcModImm(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set Thumb32DataProcPlainBin(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set Thumb32BranchesAndMiscCtrl(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set Thumb32StoreSingle(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set Thumb32LoadWord(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set Thumb32MulMulAccAndAbsDiff(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set Thumb32LongMulMulAccAndDiv(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set Thumb32DataProcReg(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set ArmMacroMem(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set M5ops(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set LoadByteMemoryHints(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set LoadHalfwordMemoryHints(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set decodeVfpData(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set decodeShortFpTransfer(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set decodeMcrMrc14(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set decodeMcrMrc15(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set decodeExtensionRegLoadStore(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set decodeNeonData(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set decodeMcrrMrrc15(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set decodeNeonThreeRegistersSameLength(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set decodeNeonOneRegModImm(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set decodeNeonTwoRegAndShift(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set decodeNeonThreeRegDiffLengths(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set decodeNeonTwoRegScalar(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);

	inline Aarch32Set decodeNeonTwoRegMisc(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType);
}

#endif //DECODE_AARCH32_H
