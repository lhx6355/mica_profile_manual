//
// Created by WenjFu on 16-11-04.
// This is for Thumb set decoding,
// Input a single inst and a counter pointer
// InstReg for register dependency 
// and InstType for statistic instruction type.
#include "misc_reg_info.h"
#include "decode_aarch32.h"
#include "decode_inst_set.h"

using namespace aarch32;

Aarch32Set aarch32::decodeT32(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType) {
	if (bits(machInst, 31, 29) != 7){
		switch (bits(machInst, 15, 13)) {
		case 0x0:
		case 0x1:
			return Thumb16ShiftAddSubMoveCmp(machInst, instSetCounters, instReg, instType);
		case 0x2:{
			switch (bits(machInst, 12, 10)) {
			case 0x0: return Thumb16DataProcessing(machInst, instSetCounters, instReg, instType);
			case 0x1: return Thumb16SpecDataAndBx(machInst, instSetCounters, instReg, instType);
			case 0x2:
			case 0x3: return Thumb16MemLit(machInst, instSetCounters, instReg, instType);
			default: return Thumb16MemReg(machInst, instSetCounters, instReg, instType);
			}
		}
		case 0x3:
		case 0x4:
			return Thumb16MemImm(machInst, instSetCounters, instReg, instType);
		case 0x5: {
			switch (bits(machInst, 12, 11)) {
			case 0x0: return Thumb16Adr(machInst, instSetCounters, instReg, instType);
			case 0x1: return Thumb16AddSp(machInst, instSetCounters, instReg, instType); //sp, immediate
			case 0x2:
			case 0x3: return Thumb16Misc(machInst, instSetCounters, instReg, instType);
			default:{++instSetCounters[Unknown32]; return Unknown32; }
			}
		}
		case 0x6:{
			switch (bits(machInst, 12, 11)) {
			case 0x0:
			case 0x1: return Thumb16MacroMem(machInst, instSetCounters, instReg, instType);
			case 0x2:
			case 0x3: return Thumb16CondBranchAndSvc(machInst, instSetCounters, instReg, instType);
			default:{++instSetCounters[Unknown32]; return Unknown32; }
			}
		}
		case 0x7:
			switch (bits(machInst, 12, 11)){
			case 0x0: return Thumb16UncondBranch(machInst, instSetCounters, instReg, instType);
			default:{++instSetCounters[Unknown32]; return Unknown32; }
			}
		default:{++instSetCounters[Unknown32]; return Unknown32; }
		}
		{++instSetCounters[Unknown32]; return Unknown32; }
	}
	// 32 bit thumb instructions.
	else{
 		uint8_t ltcoproc = bits(machInst, 11, 8);
		switch (bits(machInst, 28, 27)) {
		case 0x1:
			switch (bits(machInst, 26, 25)) {
			case 0x0:
				switch (bits(machInst, 22)) {
				case 0x0:
					switch (bits(machInst, 24, 23)) {
					case 0x0:
					case 0x3:
						return Thumb32SrsRfe(machInst, instSetCounters, instReg, instType);
						// This uses the same encoding as regular ARM.
					default:
						return ArmMacroMem(machInst, instSetCounters, instReg, instType);
					}
				case 0x1:
					return Thumb32LdrStrDExTbh(machInst, instSetCounters, instReg, instType);
				default:{++instSetCounters[Unknown32]; return Unknown32; }
				}
			case 0x1:
				return Thumb32DataProcShiftReg(machInst, instSetCounters, instReg, instType);
			default:
				switch (bits(machInst, 25, 24)) {
				case 0x2:
					switch (bits(machInst, 4)) {
					case 0x0:
						switch (ltcoproc) {
						case 0xa:
						case 0xb:
							return decodeVfpData(machInst, instSetCounters, instReg, instType);
						default: {
							++instSetCounters[WarnUnimplemented]; return WarnUnimplemented; }// WarnUnimpl::cdp(); // cdp2
						}
					case 0x1:
						switch (ltcoproc) {
						case 0x1:
							return M5ops(machInst, instSetCounters, instReg, instType);
						case 0xa:
						case 0xb:
							return decodeShortFpTransfer(machInst, instSetCounters, instReg, instType);
						case 0xe:
							return decodeMcrMrc14(machInst, instSetCounters, instReg, instType);
						case 0xf:
							return decodeMcrMrc15(machInst, instSetCounters, instReg, instType);
						default:{++instSetCounters[Unknown32]; return Unknown32; }
						}
					default:{++instSetCounters[Unknown32]; return Unknown32; }
					}
				case 0x3:
					return decodeNeonData(machInst, instSetCounters, instReg, instType);
				default:
					switch (ltcoproc) {
					case 0xa:
					case 0xb:
						return decodeExtensionRegLoadStore(machInst, instSetCounters, instReg, instType);
					case 0xf:
						switch (bits(machInst, 16 + 9, 16 + 4)) {
						case 0x00: {++instSetCounters[Unknown32]; return Unknown32; }
						case 0x04:
							switch (ltcoproc) {
							case 0xf:
								return decodeMcrrMrrc15(machInst, instSetCounters, instReg, instType);
							default: {
								++instSetCounters[WarnUnimplemented]; return WarnUnimplemented; }// WarnUnimpl::mcrr(); // mcrr2
							}
						case 0x05:
							switch (ltcoproc) {
							case 0xf:
								return decodeMcrrMrrc15(machInst, instSetCounters, instReg, instType);
							default: {
								++instSetCounters[WarnUnimplemented]; return WarnUnimplemented; }// WarnUnimpl::mrrc(); // mrrc2
							}
						case 0x02: case 0x06: case 0x08: case 0x0a: case 0x0c: case 0x0e: case 0x10:
						case 0x12: case 0x14: case 0x16: case 0x18: case 0x1a: case 0x1c: case 0x1e:{
							++instSetCounters[WarnUnimplemented]; return WarnUnimplemented; }// WarnUnimpl::stc(); // stc2
						case 0x03: case 0x07: case 0x09: case 0x0b: case 0x0d: case 0x0f: case 0x11:
						case 0x13: case 0x15: case 0x17: case 0x19: case 0x1b: case 0x1d: case 0x1f:
							switch (bits(machInst, 19, 16)) {
							case 0xf: {
								++instSetCounters[WarnUnimplemented]; return WarnUnimplemented; }// WarnUnimpl::ldc(); // ldc2 (literal)
							default: {
								++instSetCounters[WarnUnimplemented]; return WarnUnimplemented; }// WarnUnimpl::ldc(); // ldc2 (immediate)
							}
						default:{++instSetCounters[Unknown32]; return Unknown32; }
						}
					default:{++instSetCounters[Unknown32]; return Unknown32; }
					}
				}
			}
		case 0x2:
			switch (bits(machInst, 15)) {
			case 0x0:
				switch (bits(machInst, 16 + 9)) {
				case 0x0:
					return Thumb32DataProcModImm(machInst, instSetCounters, instReg, instType);
				case 0x1:
					return Thumb32DataProcPlainBin(machInst, instSetCounters, instReg, instType);
				default:{++instSetCounters[Unknown32]; return Unknown32; }
				}
			case 0x1:
				return Thumb32BranchesAndMiscCtrl(machInst, instSetCounters, instReg, instType);
			default:{++instSetCounters[Unknown32]; return Unknown32; }
			}
		case 0x3:
			switch (bits(machInst, 16 + 10, 16 + 9)) {
			case 0x0:
				switch (bits(machInst, 20)) {
				case 0x0:
					switch (bits(machInst, 16 + 8)) {
					case 0x0:
						return Thumb32StoreSingle(machInst, instSetCounters, instReg, instType);
					case 0x1:
						return decodeNeonData(machInst, instSetCounters, instReg, instType);
					default:{++instSetCounters[Unknown32]; return Unknown32; }
					}
				case 0x1:
					switch (bits(machInst, 16 + 6, 16 + 5)) {
					case 0x0:
						return LoadByteMemoryHints(machInst, instSetCounters, instReg, instType);
					case 0x1:
						return LoadHalfwordMemoryHints(machInst, instSetCounters, instReg, instType);
					case 0x2:
						return Thumb32LoadWord(machInst, instSetCounters, instReg, instType);
					default:{++instSetCounters[Unknown32]; return Unknown32; }
					}
				default:{++instSetCounters[Unknown32]; return Unknown32; }
				}
			case 0x1:
				switch (bits(machInst, 16 + 8, 16 + 7)) {
				case 0x2:
					return Thumb32MulMulAccAndAbsDiff(machInst, instSetCounters, instReg, instType);
				case 0x3:
					return Thumb32LongMulMulAccAndDiv(machInst, instSetCounters, instReg, instType);
				default:
					return Thumb32DataProcReg(machInst, instSetCounters, instReg, instType);
				}
			default:
				switch (bits(machInst, 16 + 9, 16 + 8)) {
				case 0x2:
					switch (bits(machInst, 4)) {
					case 0x0:
						switch (ltcoproc) {
						case 0xa:
						case 0xb: return decodeVfpData(machInst, instSetCounters, instReg, instType);
						default: {++instSetCounters[WarnUnimplemented]; return WarnUnimplemented; }// WarnUnimpl::cdp(); // cdp2
						}
					case 0x1: {
						switch (ltcoproc) {
						case 0x1:
							return M5ops(machInst, instSetCounters, instReg, instType);
						case 0xa:
						case 0xb:
							return decodeShortFpTransfer(machInst, instSetCounters, instReg, instType);
						case 0xe:
							return decodeMcrMrc14(machInst, instSetCounters, instReg, instType);
						case 0xf:
							return decodeMcrMrc15(machInst, instSetCounters, instReg, instType);
						default:{++instSetCounters[Unknown32]; return Unknown32; }
						}
					default:{++instSetCounters[Unknown32]; return Unknown32; }
					}
					}
				case 0x3:
					return decodeNeonData(machInst, instSetCounters, instReg, instType);
				default:
					switch (ltcoproc) {
					case 0xa:
					case 0xb: return decodeExtensionRegLoadStore(machInst, instSetCounters, instReg, instType);
					case 0xf:
						switch (bits(machInst, 16 + 9, 16 + 4)) {
						case 0x00: {++instSetCounters[Unknown32]; return Unknown32; }
						case 0x04: {
							++instSetCounters[WarnUnimplemented]; return WarnUnimplemented; }// WarnUnimpl::mcrr(); // mcrr2
						case 0x05: {
							++instSetCounters[WarnUnimplemented]; return WarnUnimplemented; }// WarnUnimpl::mrrc(); // mrrc2
						case 0x02: case 0x06: case 0x08: case 0x0a: case 0x0c: case 0x0e: case 0x10:
						case 0x12: case 0x14: case 0x16: case 0x18: case 0x1a: case 0x1c: case 0x1e:{
							++instSetCounters[WarnUnimplemented]; return WarnUnimplemented; }// WarnUnimpl::stc(); // stc2
						case 0x03: case 0x07: case 0x09: case 0x0b: case 0x0d: case 0x0f: case 0x11:
						case 0x13: case 0x15: case 0x17: case 0x19: case 0x1b: case 0x1d: case 0x1f:
							switch (bits(machInst, 19, 16)) {
							case 0xf:{
								++instSetCounters[WarnUnimplemented]; return WarnUnimplemented; }// WarnUnimpl::ldc(); // ldc2 (literal)
							default:{
								++instSetCounters[WarnUnimplemented]; return WarnUnimplemented; }// WarnUnimpl::ldc(); // ldc2 (immediate)
							}
						default:{++instSetCounters[Unknown32]; return Unknown32; }
						}
					default:{++instSetCounters[Unknown32]; return Unknown32; }
					}
				}
			}
		default:{++instSetCounters[Unknown32]; return Unknown32; }
		}
	}
	{++instSetCounters[Unknown32]; return Unknown32; }
}


Aarch32Set aarch32::decodeA32(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType) {
	switch (COND_CODE) {
	case 0xf:
		// ArmUnconditional::armUnconditional(([], {}))
	{
		const IntRegIndex rn = (IntRegIndex)(uint32_t)
			bits(machInst, 19, 16);
		instReg.SReg[0] = rn;
		const uint32_t op1 = bits(machInst, 27, 20);
		if (bits(op1, 7) == 0) {
			const uint32_t op2 = bits(machInst, 7, 4);
			if (op1 == 0x10) {
				instType.isCc = true;
				if (bits((uint32_t)rn, 0) == 1 && op2 == 0) {
					++instSetCounters[Setend];
					return Setend;
				}
				else if (bits((uint32_t)rn, 0) == 0 && bits(op2, 1) == 0) {
					instType.isMISC = true;
					instType.isBranch = true;
					++instSetCounters[Cps];
					return Cps;
				}
			}
			else if (bits(op1, 6, 5) == 0x1) {
				return decodeNeonData(machInst, instSetCounters, instReg, instType);
			}
			else if (bits(op1, 6, 4) == 0x4) {
				if (bits(op1, 0) == 0) {
					// remain problem 
					return aarch32::decodeNeonMem(machInst, instSetCounters, instReg, instType);
				}
				else if (bits(op1, 2, 0) == 1) {
					// Unallocated memory hint
					//instType.isBranch = true;
					++instSetCounters[NopInst];
					return NopInst;
				}
				else if (bits(op1, 2, 0) == 5) {
					const bool add = bits(machInst, 23);
					const uint32_t imm12 = bits(machInst, 11, 0);
					instType.isLoad = true;
					if (add) {
						++instSetCounters[PLI_LOAD_IMM_AY_PN_SN_UN_WN_SZ1];
						return PLI_LOAD_IMM_AY_PN_SN_UN_WN_SZ1;
					}
					else {
						++instSetCounters[PLI_LOAD_IMM_AN_PN_SN_UN_WN_SZ1];
						return PLI_LOAD_IMM_AN_PN_SN_UN_WN_SZ1;
					}
				}
			}
			else if (bits(op1, 6, 4) == 0x5) {
				if (bits(op1, 1, 0) == 0x1) {
					const bool add = bits(machInst, 23);
					const bool pldw = bits(machInst, 22);
					const uint32_t imm12 = bits(machInst, 11, 0);
					instType.isLoad = true;
					if (pldw) {
						if (add) {
							++instSetCounters[PLDW_LOAD_IMM_AY_PN_SN_UN_WN_SZ1];
							return PLDW_LOAD_IMM_AY_PN_SN_UN_WN_SZ1;
						}
						else {
							++instSetCounters[PLDW_LOAD_IMM_AN_PN_SN_UN_WN_SZ1];
							return PLDW_LOAD_IMM_AN_PN_SN_UN_WN_SZ1;
						}
					}
					else {
						if (add) {
							++instSetCounters[PLD_LOAD_IMM_AY_PN_SN_UN_WN_SZ1];
							return PLD_LOAD_IMM_AY_PN_SN_UN_WN_SZ1;
						}
						else {
							++instSetCounters[PLD_LOAD_IMM_AN_PN_SN_UN_WN_SZ1];
							return PLD_LOAD_IMM_AN_PN_SN_UN_WN_SZ1;
						}
					}
				}
				else if (op1 == 0x57) {
					instType.isMISC = true;
					instType.isSerial = true;
					switch (op2) {
					case 0x1:
						++instSetCounters[Clrex];
						return Clrex;
					case 0x4:
						++instSetCounters[Dsb];
						return Dsb;
					case 0x5:
						++instSetCounters[Dmb];
						return Dmb;
					case 0x6:
						++instSetCounters[Isb];
						return Isb;
					}
				}
			}
			else if (bits(op2, 0) == 0) {
				switch (op1 & 0xf7) {
				case 0x61:
					// Unallocated memory hint
					//instType.isBranch = true;
					++instSetCounters[NopInst];
					return NopInst;
				case 0x65: {
					const uint32_t imm5 = bits(machInst, 11, 7);
					const uint32_t type = bits(machInst, 6, 5);
					const bool add = bits(machInst, 23);
					const IntRegIndex rm = (IntRegIndex)(uint32_t)
						bits(machInst, 3, 0);
					instReg.SReg[1] = rm;
					instType.isLoad = true;
					if (add) {
						++instSetCounters[PLI_LOAD_REG_AY_PN_SN_UN_WN_SZ1];
						return PLI_LOAD_REG_AY_PN_SN_UN_WN_SZ1;
					}
					else {
						++instSetCounters[PLI_LOAD_REG_AN_PN_SN_UN_WN_SZ1];
						return PLI_LOAD_REG_AN_PN_SN_UN_WN_SZ1;
					}
				}
				case 0x71:
				case 0x75: {
					const uint32_t imm5 = bits(machInst, 11, 7);
					const uint32_t type = bits(machInst, 6, 5);
					const bool add = bits(machInst, 23);
					const bool pldw = bits(machInst, 22);
					const IntRegIndex rm = (IntRegIndex)(uint32_t)
						bits(machInst, 3, 0);
					instReg.SReg[1] = rm;
					instType.isLoad = true;
					if (pldw) {
						if (add) {
							++instSetCounters[PLDW_LOAD_REG_AY_PN_SN_UN_WN_SZ1];
							return PLDW_LOAD_REG_AY_PN_SN_UN_WN_SZ1;
						}
						else {
							++instSetCounters[PLDW_LOAD_REG_AN_PN_SN_UN_WN_SZ1];
							return PLDW_LOAD_REG_AN_PN_SN_UN_WN_SZ1;
						}
					}
					else {
						if (add) {
							++instSetCounters[PLD_LOAD_REG_AY_PN_SN_UN_WN_SZ1];
							return PLD_LOAD_REG_AY_PN_SN_UN_WN_SZ1;
						}
						else {
							++instSetCounters[PLD_LOAD_REG_AN_PN_SN_UN_WN_SZ1];
							return PLD_LOAD_REG_AN_PN_SN_UN_WN_SZ1;
						}
					}
				}
				}
			}
		}
		else {
			switch (bits(machInst, 26, 25)) {
			case 0x0: {
				const uint32_t val = ((machInst >> 20) & 0x5);
				if (val == 0x4) {
					const uint32_t mode = bits(machInst, 4, 0);
					if (badMode32((OperatingMode)mode))
						++instSetCounters[Unknown32];
					return Unknown32;
					switch (bits(machInst, 24, 21)) {
					case 0x2:
						instType.isStore = true;
						++instSetCounters[SRS_STORE_IMM_AN_PY_SN_UN_WN_SZ8];
						return SRS_STORE_IMM_AN_PY_SN_UN_WN_SZ8;
						++instSetCounters[SRS_STORE_IMM_AN_PY_SN_UN_WY_SZ8];
						return SRS_STORE_IMM_AN_PY_SN_UN_WY_SZ8;
						++instSetCounters[SRS_STORE_IMM_AY_PY_SN_UN_WN_SZ8];
						return SRS_STORE_IMM_AY_PY_SN_UN_WN_SZ8;
						++instSetCounters[SRS_STORE_IMM_AY_PY_SN_UN_WY_SZ8];
						return SRS_STORE_IMM_AY_PY_SN_UN_WY_SZ8;
						++instSetCounters[SRS_STORE_IMM_AN_PN_SN_UN_WN_SZ8];
						return SRS_STORE_IMM_AN_PN_SN_UN_WN_SZ8;
						++instSetCounters[SRS_STORE_IMM_AN_PN_SN_UN_WY_SZ8];
						return SRS_STORE_IMM_AN_PN_SN_UN_WY_SZ8;
						++instSetCounters[SRS_STORE_IMM_AY_PN_SN_UN_WN_SZ8];
						return SRS_STORE_IMM_AY_PN_SN_UN_WN_SZ8;
						++instSetCounters[SRS_STORE_IMM_AY_PN_SN_UN_WY_SZ8];
						return SRS_STORE_IMM_AY_PN_SN_UN_WY_SZ8;
						++instSetCounters[Unknown32];
						return Unknown32;
					}
				}
				else if (val == 0x1) {
					switch (bits(machInst, 24, 21)) {
						instType.isLoad = true;
					case 0x0:
						++instSetCounters[RFE_LOAD_IMM_AN_PY_SN_UN_WN_SZ8];
						return RFE_LOAD_IMM_AN_PY_SN_UN_WN_SZ8;
					case 0x1:
						++instSetCounters[RFE_LOAD_IMM_AN_PY_SN_UN_WY_SZ8];
						return RFE_LOAD_IMM_AN_PY_SN_UN_WY_SZ8;
					case 0x4:
						++instSetCounters[RFE_LOAD_IMM_AY_PY_SN_UN_WN_SZ8];
						return RFE_LOAD_IMM_AY_PY_SN_UN_WN_SZ8;
					case 0x5:
						++instSetCounters[RFE_LOAD_IMM_AY_PY_SN_UN_WY_SZ8];
						return RFE_LOAD_IMM_AY_PY_SN_UN_WY_SZ8;
					case 0x8:
						++instSetCounters[RFE_LOAD_IMM_AN_PN_SN_UN_WN_SZ8];
						return RFE_LOAD_IMM_AN_PN_SN_UN_WN_SZ8;
					case 0x9:
						++instSetCounters[RFE_LOAD_IMM_AN_PN_SN_UN_WY_SZ8];
						return RFE_LOAD_IMM_AN_PN_SN_UN_WY_SZ8;
					case 0xc:
						++instSetCounters[RFE_LOAD_IMM_AY_PN_SN_UN_WN_SZ8];
						return RFE_LOAD_IMM_AY_PN_SN_UN_WN_SZ8;
					case 0xd:
						++instSetCounters[RFE_LOAD_IMM_AY_PN_SN_UN_WY_SZ8];
						return RFE_LOAD_IMM_AY_PN_SN_UN_WY_SZ8;
					}
					++instSetCounters[Unknown32];
					return Unknown32;
				}
			}
					  break;
			case 0x1: {
				instType.isBranch = true;
				const uint32_t imm = (sext<26>(bits(machInst, 23, 0) << 2)) | (bits(machInst, 24) << 1);
				++instSetCounters[BlxImm];
				return BlxImm;
			}
			case 0x2:
				if (bits(op1, 4, 0) != 0) {
					if (CPNUM == 0xa || CPNUM == 0xb) {
						return decodeExtensionRegLoadStore(machInst, instSetCounters, instReg, instType);
					}
					if (bits(op1, 0) == 1) {
						if (rn == INTREG_PC) {
							if (bits(op1, 4, 3) != 0x0) {
								++instSetCounters[WarnUnimplemented];
								return WarnUnimplemented;
							}
						}
						else {
							if (op1 == 0xC3 || op1 == 0xC7) {
								++instSetCounters[WarnUnimplemented];
								return WarnUnimplemented;
							}
						}
					}
					else {
						if (bits(op1, 4, 3) != 0 || bits(op1, 1) == 1) {
							++instSetCounters[WarnUnimplemented];
							return WarnUnimplemented;
						}
					}
				}
				break;
			case 0x3:
				if (bits(op1, 4) == 0) {
					if (CPNUM == 0xa || CPNUM == 0xb) {
						return decodeShortFpTransfer(machInst, instSetCounters, instReg, instType);
					}
					else if (CPNUM == 0xe) {
						return decodeMcrMrc14(machInst, instSetCounters, instReg, instType);
					}
					else if (CPNUM == 0xf) {
						return decodeMcrMrc15(machInst, instSetCounters, instReg, instType);
					}
					const bool op = bits(machInst, 4);
					if (op) {
						if (bits(op1, 0)) {
							++instSetCounters[WarnUnimplemented];
							return WarnUnimplemented;
						}
						else {
							++instSetCounters[WarnUnimplemented];
							return WarnUnimplemented;
						}
					}
					else {
						++instSetCounters[WarnUnimplemented];
						return WarnUnimplemented;
					}
				}
				break;
			}
		}
		++instSetCounters[Unknown32];
		return Unknown32;
	}
	break;

	default:
		switch (ENCODING) {

		case 0x0:
			switch (SEVEN_AND_FOUR) {

			case 0x1:
				switch (MISC_OPCODE) {

				case 0x9:
					switch (PREPOST) {

					case 0x0:
						// ArmMultAndMultAcc::armMultAndMultAcc(([], {}))

					{
						// The manual defines this field as 23-20, but bit 20 is usually
						// ignored.
						const uint32_t op = bits(machInst, 23, 21);
						const bool s = bits(machInst, 20);
						const IntRegIndex rd = (IntRegIndex)(uint32_t)bits(machInst, 19, 16);
						const IntRegIndex ra = (IntRegIndex)(uint32_t)bits(machInst, 15, 12);
						const IntRegIndex rm = (IntRegIndex)(uint32_t)bits(machInst, 11, 8);
						const IntRegIndex rn = (IntRegIndex)(uint32_t)bits(machInst, 3, 0);
						instReg.DReg[0] = rd;
						instReg.SReg[0] = rn;
						instReg.SReg[1] = rm;
						switch (op) {
						case 0x0:
							if (s) {
								instType.isCc = true;
								++instSetCounters[MulCc]; return MulCc;
							}
							else {
								++instSetCounters[Mul]; return Mul;
							}
						case 0x1:
							instReg.SReg[2] = ra;
							if (s) {
								instType.isCc = true;
								++instSetCounters[MlaCc]; return MlaCc;
							}
							else {
								++instSetCounters[Mla]; return Mla;
							}
						case 0x2:
							instReg.SReg[2] = ra;
							++instSetCounters[Umaal]; return Umaal;
						case 0x3:
							instReg.SReg[2] = ra;
							++instSetCounters[Mls]; return Mls;
						case 0x4:
							instReg.SReg[2] = ra;
							if (s) {
								instType.isCc = true;
								++instSetCounters[UmullCc]; return UmullCc;
							}
							else {
								instReg.SReg[2] = ra;
								++instSetCounters[Umull]; return Umull;
							}
						case 0x5:
							instReg.SReg[2] = ra;
							instReg.DReg[1] = rm;
							if (s) {
								instType.isCc = true;
								++instSetCounters[UmlalCc]; return UmlalCc;
							}
							else {
								++instSetCounters[Umlal]; return Umlal;
							}
						case 0x6:
							instReg.DReg[1] = ra;
							if (s) {
								instType.isCc = true;
								++instSetCounters[SmullCc]; return SmullCc;
							}
							else {
								++instSetCounters[Smull]; return Smull;
							}
						case 0x7:
							instReg.DReg[1] = ra;
							if (s) {
								instType.isCc = true;
								++instSetCounters[SmlalCc]; return SmlalCc;
							}
							else {
								++instSetCounters[Smlal]; return Smlal;
							}
						}
					}
					break;

					case 0x1:
						// ArmSyncMem::armSyncMem(([], {}))

					{
						const IntRegIndex rn = (IntRegIndex)(uint32_t)bits(machInst, 19, 16);
						const IntRegIndex rt = (IntRegIndex)(uint32_t)bits(machInst, 15, 12);
						const IntRegIndex rt2 = (IntRegIndex)(uint32_t)bits(machInst, 3, 0);
						instReg.SReg[0] = rn;
						switch (PUBWL) {
						case 0x10:
							++instSetCounters[Swp]; return Swp;
						case 0x14:
							++instSetCounters[Swp]; return Swp;
						case 0x18:
							instType.isStore = true;
							instReg.DReg[1] = rt;
							++instSetCounters[STREX_STORE_IMM_AY_PN_SN_UN_WN_SZ4]; return STREX_STORE_IMM_AY_PN_SN_UN_WN_SZ4;
						case 0x19:
							instReg.DReg[0] = rt;
							instReg.SReg[1] = rt2;
							instType.isLoad = true;
							++instSetCounters[LDREX_LOAD_IMM_AY_PN_SN_UN_WN_SZ4]; return LDREX_LOAD_IMM_AY_PN_SN_UN_WN_SZ4;
						case 0x1a:
							instReg.DReg[0] = rt;
							instReg.SReg[1] = rt2;
							instType.isStore = true;
							++instSetCounters[STREXD_STORE_IMMD_AY_PN_SN_UN_WN_SZ4]; return STREXD_STORE_IMMD_AY_PN_SN_UN_WN_SZ4;
						case 0x1b:
							instType.isLoad = true;
							instReg.DReg[0] = rt;
							instReg.DReg[1] = rt2;
							++instSetCounters[LDREXD_LOAD_IMMD_AY_PN_SN_UN_WN_SZ4]; return LDREXD_LOAD_IMMD_AY_PN_SN_UN_WN_SZ4;
						case 0x1c:
							instReg.DReg[0] = rt;
							instReg.SReg[1] = rt2;
							instType.isStore = true;
							++instSetCounters[STREXB_STORE_IMM_AY_PN_SN_UN_WN_SZ1]; return STREXB_STORE_IMM_AY_PN_SN_UN_WN_SZ1;
						case 0x1d:
							instReg.DReg[0] = rt;
							instType.isLoad = true;
							++instSetCounters[LDREXB_LOAD_IMM_AY_PN_SN_UN_WN_SZ1]; return LDREXB_LOAD_IMM_AY_PN_SN_UN_WN_SZ1;
						case 0x1e:
							instReg.DReg[0] = rt;
							instReg.SReg[1] = rt2;
							instType.isStore = true;
							++instSetCounters[STREXH_STORE_IMM_AY_PN_SN_UN_WN_SZ2]; return STREXH_STORE_IMM_AY_PN_SN_UN_WN_SZ2;
						case 0x1f:
							instReg.DReg[0] = rt;
							instType.isLoad = true;
							++instSetCounters[LDREXH_LOAD_IMM_AY_PN_SN_UN_WN_SZ2]; return LDREXH_LOAD_IMM_AY_PN_SN_UN_WN_SZ2;
						default:
							++instSetCounters[Unknown32]; return Unknown32;
						}
					}
					break;

					default:

						// Unknown32::unknown(([], {}))
						++instSetCounters[Unknown32]; return Unknown32;
						break;
					}

				case 0xb: case 0xd: case 0xf:
					// AddrMode3::addrMode3(([], {}))

				{
					const uint32_t op1 = bits(machInst, 24, 20);
					const uint32_t op2 = bits(machInst, 6, 5);
					const uint32_t puiw = bits(machInst, 24, 21);
					const uint32_t imm = IMMED_HI_11_8 << 4 | IMMED_LO_3_0;
					switch (op2) {
					case 0x1:
						if (op1 & 0x1) {
							instType.isLoad = true;
							switch (puiw) {
							case 0x0:
								instReg.DReg[0] = RT;
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								++instSetCounters[LOAD_REG_AN_PY_SN_UN_WY_SZ2]; return LOAD_REG_AN_PY_SN_UN_WY_SZ2;

							case 0x1:
								instReg.DReg[0] = RT;
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								++instSetCounters[LOAD_REG_AN_PY_SN_UY_WY_SZ2]; return LOAD_REG_AN_PY_SN_UY_WY_SZ2;

							case 0x2:
								instReg.DReg[0] = RT;
								instReg.SReg[0] = RN;
								++instSetCounters[LOAD_IMM_AN_PY_SN_UN_WY_SZ2]; return LOAD_IMM_AN_PY_SN_UN_WY_SZ2;

							case 0x3:
								instReg.DReg[0] = RT;
								instReg.SReg[0] = RN;
								++instSetCounters[LOAD_IMM_AN_PY_SN_UY_WY_SZ2]; return LOAD_IMM_AN_PY_SN_UY_WY_SZ2;

							case 0x4:
								instReg.DReg[0] = RT;
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								++instSetCounters[LOAD_REG_AY_PY_SN_UN_WY_SZ2]; return LOAD_REG_AY_PY_SN_UN_WY_SZ2;

							case 0x5:
								instReg.DReg[0] = RT;
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								++instSetCounters[LOAD_REG_AY_PY_SN_UY_WY_SZ2]; return LOAD_REG_AY_PY_SN_UY_WY_SZ2;

							case 0x6:
								instReg.DReg[0] = RT;
								instReg.SReg[0] = RN;
								++instSetCounters[LOAD_IMM_AY_PY_SN_UN_WY_SZ2]; return LOAD_IMM_AY_PY_SN_UN_WY_SZ2;

							case 0x7:
								instReg.DReg[0] = RT;
								instReg.SReg[0] = RN;
								++instSetCounters[LOAD_IMM_AY_PY_SN_UY_WY_SZ2]; return LOAD_IMM_AY_PY_SN_UY_WY_SZ2;

							case 0x8:
								instReg.DReg[0] = RT;
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								++instSetCounters[LOAD_REG_AN_PN_SN_UN_WN_SZ2]; return LOAD_REG_AN_PN_SN_UN_WN_SZ2;

							case 0x9:
								instReg.DReg[0] = RT;
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								++instSetCounters[LOAD_REG_AN_PN_SN_UN_WY_SZ2]; return LOAD_REG_AN_PN_SN_UN_WY_SZ2;

							case 0xa:
								instReg.DReg[0] = RT;
								instReg.SReg[0] = RN;
								++instSetCounters[LOAD_IMM_AN_PN_SN_UN_WN_SZ2]; return LOAD_IMM_AN_PN_SN_UN_WN_SZ2;

							case 0xb:
								instReg.DReg[0] = RT;
								instReg.SReg[0] = RN;
								++instSetCounters[LOAD_IMM_AN_PN_SN_UN_WY_SZ2]; return LOAD_IMM_AN_PN_SN_UN_WY_SZ2;

							case 0xc:
								instReg.DReg[0] = RT;
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								++instSetCounters[LOAD_REG_AY_PN_SN_UN_WN_SZ2]; return LOAD_REG_AY_PN_SN_UN_WN_SZ2;

							case 0xd:
								instReg.DReg[0] = RT;
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								++instSetCounters[LOAD_REG_AY_PN_SN_UN_WY_SZ2]; return LOAD_REG_AY_PN_SN_UN_WY_SZ2;

							case 0xe:
								instReg.DReg[0] = RT;
								instReg.SReg[0] = RN;
								++instSetCounters[LOAD_IMM_AY_PN_SN_UN_WN_SZ2]; return LOAD_IMM_AY_PN_SN_UN_WN_SZ2;

							case 0xf:
								instReg.DReg[0] = RT;
								instReg.SReg[0] = RN;
								++instSetCounters[LOAD_IMM_AY_PN_SN_UN_WY_SZ2]; return LOAD_IMM_AY_PN_SN_UN_WY_SZ2;

							default:
								++instSetCounters[Unknown32]; return Unknown32;
							}
						}
						else {
							switch (puiw) {
								instType.isStore = true;
							case 0x0:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.SReg[2] = RT;
								++instSetCounters[STORE_REG_AN_PY_SN_UN_WY_SZ2]; return STORE_REG_AN_PY_SN_UN_WY_SZ2;

							case 0x1:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.SReg[2] = RT;
								++instSetCounters[STORE_REG_AN_PY_SN_UY_WY_SZ2]; return STORE_REG_AN_PY_SN_UY_WY_SZ2;

							case 0x2:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RT;
								++instSetCounters[STORE_IMM_AN_PY_SN_UN_WY_SZ2]; return STORE_IMM_AN_PY_SN_UN_WY_SZ2;

							case 0x3:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RT;
								++instSetCounters[STORE_IMM_AN_PY_SN_UY_WY_SZ2]; return STORE_IMM_AN_PY_SN_UY_WY_SZ2;

							case 0x4:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.SReg[2] = RT;
								++instSetCounters[STORE_REG_AY_PY_SN_UN_WY_SZ2]; return STORE_REG_AY_PY_SN_UN_WY_SZ2;

							case 0x5:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.SReg[2] = RT;
								++instSetCounters[STORE_REG_AY_PY_SN_UY_WY_SZ2]; return STORE_REG_AY_PY_SN_UY_WY_SZ2;

							case 0x6:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RT;
								++instSetCounters[STORE_IMM_AY_PY_SN_UN_WY_SZ2]; return STORE_IMM_AY_PY_SN_UN_WY_SZ2;

							case 0x7:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RT;
								++instSetCounters[STORE_IMM_AY_PY_SN_UY_WY_SZ2]; return STORE_IMM_AY_PY_SN_UY_WY_SZ2;

							case 0x8:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.SReg[2] = RT;
								++instSetCounters[STORE_REG_AN_PN_SN_UN_WN_SZ2]; return STORE_REG_AN_PN_SN_UN_WN_SZ2;

							case 0x9:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.SReg[2] = RT;
								++instSetCounters[STORE_REG_AN_PN_SN_UN_WY_SZ2]; return STORE_REG_AN_PN_SN_UN_WY_SZ2;

							case 0xa:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RT;
								++instSetCounters[STORE_IMM_AN_PN_SN_UN_WN_SZ2]; return STORE_IMM_AN_PN_SN_UN_WN_SZ2;

							case 0xb:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RT;
								++instSetCounters[STORE_IMM_AN_PN_SN_UN_WY_SZ2]; return STORE_IMM_AN_PN_SN_UN_WY_SZ2;

							case 0xc:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.SReg[2] = RT;
								++instSetCounters[STORE_REG_AY_PN_SN_UN_WN_SZ2]; return STORE_REG_AY_PN_SN_UN_WN_SZ2;

							case 0xd:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.SReg[2] = RT;
								++instSetCounters[STORE_REG_AY_PN_SN_UN_WY_SZ2]; return STORE_REG_AY_PN_SN_UN_WY_SZ2;

							case 0xe:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RT;
								++instSetCounters[STORE_IMM_AY_PN_SN_UN_WN_SZ2]; return STORE_IMM_AY_PN_SN_UN_WN_SZ2;

							case 0xf:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RT;
								++instSetCounters[STORE_IMM_AY_PN_SN_UN_WY_SZ2]; return STORE_IMM_AY_PN_SN_UN_WY_SZ2;

							default:
								++instSetCounters[Unknown32]; return Unknown32;
							}
						}
					case 0x2:
						if (op1 & 0x1) {
							instType.isLoad = true;
							switch (puiw) {
							case 0x0:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_REG_AN_PY_SY_UN_WY_SZ1]; return LOAD_REG_AN_PY_SY_UN_WY_SZ1;

							case 0x1:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_REG_AN_PY_SY_UY_WY_SZ1]; return LOAD_REG_AN_PY_SY_UY_WY_SZ1;

							case 0x2:
								instReg.SReg[0] = RN;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_IMM_AN_PY_SY_UN_WY_SZ1]; return LOAD_IMM_AN_PY_SY_UN_WY_SZ1;

							case 0x3:
								instReg.SReg[0] = RN;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_IMM_AN_PY_SY_UY_WY_SZ1]; return LOAD_IMM_AN_PY_SY_UY_WY_SZ1;

							case 0x4:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_REG_AY_PY_SY_UN_WY_SZ1]; return LOAD_REG_AY_PY_SY_UN_WY_SZ1;

							case 0x5:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_REG_AY_PY_SY_UY_WY_SZ1]; return LOAD_REG_AY_PY_SY_UY_WY_SZ1;

							case 0x6:
								instReg.SReg[0] = RN;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_IMM_AY_PY_SY_UN_WY_SZ1]; return LOAD_IMM_AY_PY_SY_UN_WY_SZ1;

							case 0x7:
								instReg.SReg[0] = RN;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_IMM_AY_PY_SY_UY_WY_SZ1]; return LOAD_IMM_AY_PY_SY_UY_WY_SZ1;

							case 0x8:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_REG_AN_PN_SY_UN_WN_SZ1]; return LOAD_REG_AN_PN_SY_UN_WN_SZ1;

							case 0x9:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_REG_AN_PN_SY_UN_WY_SZ1]; return LOAD_REG_AN_PN_SY_UN_WY_SZ1;

							case 0xa:
								instReg.SReg[0] = RN;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_IMM_AN_PN_SY_UN_WN_SZ1]; return LOAD_IMM_AN_PN_SY_UN_WN_SZ1;

							case 0xb:
								instReg.SReg[0] = RN;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_IMM_AN_PN_SY_UN_WY_SZ1]; return LOAD_IMM_AN_PN_SY_UN_WY_SZ1;

							case 0xc:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_REG_AY_PN_SY_UN_WN_SZ1]; return LOAD_REG_AY_PN_SY_UN_WN_SZ1;

							case 0xd:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_REG_AY_PN_SY_UN_WY_SZ1]; return LOAD_REG_AY_PN_SY_UN_WY_SZ1;

							case 0xe:
								instReg.SReg[0] = RN;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_IMM_AY_PN_SY_UN_WN_SZ1]; return LOAD_IMM_AY_PN_SY_UN_WN_SZ1;

							case 0xf:
								instReg.SReg[0] = RN;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_IMM_AY_PN_SY_UN_WY_SZ1]; return LOAD_IMM_AY_PN_SY_UN_WY_SZ1;

							default:
								++instSetCounters[Unknown32]; return Unknown32;
							}

						}
						else if ((RT % 2) == 0) {
							instType.isLoad = true;
							switch (puiw) {
							case 0x0:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.DReg[0] = RT & ~1;
								instReg.DReg[1] = RT | 1;
								++instSetCounters[LOAD_REGD_AN_PY_SN_UN_WY_SZ4]; return LOAD_REGD_AN_PY_SN_UN_WY_SZ4;

							case 0x1:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.DReg[0] = RT & ~1;
								instReg.DReg[1] = RT | 1;
								++instSetCounters[LOAD_REGD_AN_PY_SN_UN_WY_SZ4]; return LOAD_REGD_AN_PY_SN_UN_WY_SZ4;

							case 0x2:
								instReg.SReg[0] = RN;
								instReg.DReg[0] = RT & ~1;
								instReg.DReg[1] = RT | 1;
								++instSetCounters[LOAD_IMMD_AN_PY_SN_UN_WY_SZ4]; return LOAD_IMMD_AN_PY_SN_UN_WY_SZ4;

							case 0x3:
								instReg.SReg[0] = RN;
								instReg.DReg[0] = RT & ~1;
								instReg.DReg[1] = RT | 1;
								++instSetCounters[LOAD_IMMD_AN_PY_SN_UN_WY_SZ4]; return LOAD_IMMD_AN_PY_SN_UN_WY_SZ4;

							case 0x4:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.DReg[0] = RT & ~1;
								instReg.DReg[1] = RT | 1;
								++instSetCounters[LOAD_REGD_AY_PY_SN_UN_WY_SZ4]; return LOAD_REGD_AY_PY_SN_UN_WY_SZ4;

							case 0x5:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.DReg[0] = RT & ~1;
								instReg.DReg[1] = RT | 1;
								++instSetCounters[LOAD_REGD_AY_PY_SN_UN_WY_SZ4]; return LOAD_REGD_AY_PY_SN_UN_WY_SZ4;

							case 0x6:
								instReg.SReg[0] = RN;
								instReg.DReg[0] = RT & ~1;
								instReg.DReg[1] = RT | 1;
								++instSetCounters[LOAD_IMMD_AY_PY_SN_UN_WY_SZ4]; return LOAD_IMMD_AY_PY_SN_UN_WY_SZ4;

							case 0x7:
								instReg.SReg[0] = RN;
								instReg.DReg[0] = RT & ~1;
								instReg.DReg[1] = RT | 1;
								++instSetCounters[LOAD_IMMD_AY_PY_SN_UN_WY_SZ4]; return LOAD_IMMD_AY_PY_SN_UN_WY_SZ4;

							case 0x8:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.DReg[0] = RT & ~1;
								instReg.DReg[1] = RT | 1;
								++instSetCounters[LOAD_REGD_AN_PN_SN_UN_WN_SZ4]; return LOAD_REGD_AN_PN_SN_UN_WN_SZ4;

							case 0x9:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.DReg[0] = RT & ~1;
								instReg.DReg[1] = RT | 1;
								++instSetCounters[LOAD_REGD_AN_PN_SN_UN_WY_SZ4]; return LOAD_REGD_AN_PN_SN_UN_WY_SZ4;

							case 0xa:
								instReg.SReg[0] = RN;
								instReg.DReg[0] = RT & ~1;
								instReg.DReg[1] = RT | 1;
								++instSetCounters[LOAD_IMMD_AN_PN_SN_UN_WN_SZ4]; return LOAD_IMMD_AN_PN_SN_UN_WN_SZ4;

							case 0xb:
								instReg.SReg[0] = RN;
								instReg.DReg[0] = RT & ~1;
								instReg.DReg[1] = RT | 1;
								++instSetCounters[LOAD_IMMD_AN_PN_SN_UN_WY_SZ4]; return LOAD_IMMD_AN_PN_SN_UN_WY_SZ4;

							case 0xc:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.DReg[0] = RT & ~1;
								instReg.DReg[1] = RT | 1;
								++instSetCounters[LOAD_REGD_AY_PN_SN_UN_WN_SZ4]; return LOAD_REGD_AY_PN_SN_UN_WN_SZ4;

							case 0xd:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.DReg[0] = RT & ~1;
								instReg.DReg[1] = RT | 1;
								++instSetCounters[LOAD_REGD_AY_PN_SN_UN_WY_SZ4]; return LOAD_REGD_AY_PN_SN_UN_WY_SZ4;

							case 0xe:
								instReg.SReg[0] = RN;
								instReg.DReg[0] = RT & ~1;
								instReg.DReg[1] = RT | 1;
								++instSetCounters[LOAD_IMMD_AY_PN_SN_UN_WN_SZ4]; return LOAD_IMMD_AY_PN_SN_UN_WN_SZ4;

							case 0xf:
								instReg.SReg[0] = RN;
								instReg.DReg[0] = RT & ~1;
								instReg.DReg[1] = RT | 1;
								++instSetCounters[LOAD_IMMD_AY_PN_SN_UN_WY_SZ4]; return LOAD_IMMD_AY_PN_SN_UN_WY_SZ4;

							default:
								++instSetCounters[Unknown32]; return Unknown32;
							}

						}
						else {
							++instSetCounters[Unknown32]; return Unknown32;
						}
					case 0x3:
						if (op1 & 0x1) {
							instType.isLoad = true;
							switch (puiw) {
							case 0x0:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_REG_AN_PY_SY_UN_WY_SZ2]; return LOAD_REG_AN_PY_SY_UN_WY_SZ2;

							case 0x1:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_REG_AN_PY_SY_UY_WY_SZ2]; return LOAD_REG_AN_PY_SY_UY_WY_SZ2;

							case 0x2:
								instReg.SReg[0] = RN;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_IMM_AN_PY_SY_UN_WY_SZ2]; return LOAD_IMM_AN_PY_SY_UN_WY_SZ2;

							case 0x3:
								instReg.SReg[0] = RN;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_IMM_AN_PY_SY_UY_WY_SZ2]; return LOAD_IMM_AN_PY_SY_UY_WY_SZ2;

							case 0x4:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_REG_AY_PY_SY_UN_WY_SZ2]; return LOAD_REG_AY_PY_SY_UN_WY_SZ2;

							case 0x5:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_REG_AY_PY_SY_UY_WY_SZ2]; return LOAD_REG_AY_PY_SY_UY_WY_SZ2;

							case 0x6:
								instReg.SReg[0] = RN;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_IMM_AY_PY_SY_UN_WY_SZ2]; return LOAD_IMM_AY_PY_SY_UN_WY_SZ2;

							case 0x7:
								instReg.SReg[0] = RN;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_IMM_AY_PY_SY_UY_WY_SZ2]; return LOAD_IMM_AY_PY_SY_UY_WY_SZ2;

							case 0x8:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_REG_AN_PN_SY_UN_WN_SZ2]; return LOAD_REG_AN_PN_SY_UN_WN_SZ2;

							case 0x9:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_REG_AN_PN_SY_UN_WY_SZ2]; return LOAD_REG_AN_PN_SY_UN_WY_SZ2;

							case 0xa:
								instReg.SReg[0] = RN;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_IMM_AN_PN_SY_UN_WN_SZ2]; return LOAD_IMM_AN_PN_SY_UN_WN_SZ2;

							case 0xb:
								instReg.SReg[0] = RN;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_IMM_AN_PN_SY_UN_WY_SZ2]; return LOAD_IMM_AN_PN_SY_UN_WY_SZ2;

							case 0xc:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_REG_AY_PN_SY_UN_WN_SZ2]; return LOAD_REG_AY_PN_SY_UN_WN_SZ2;

							case 0xd:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_REG_AY_PN_SY_UN_WY_SZ2]; return LOAD_REG_AY_PN_SY_UN_WY_SZ2;

							case 0xe:
								instReg.SReg[0] = RN;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_IMM_AY_PN_SY_UN_WN_SZ2]; return LOAD_IMM_AY_PN_SY_UN_WN_SZ2;

							case 0xf:
								instReg.SReg[0] = RN;
								instReg.DReg[0] = RT;
								++instSetCounters[LOAD_IMM_AY_PN_SY_UN_WY_SZ2]; return LOAD_IMM_AY_PN_SY_UN_WY_SZ2;

							default:
								++instSetCounters[Unknown32]; return Unknown32;
							}

						}
						else {
							switch (puiw) { // need 4 source register

							case 0x0:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.SReg[2] = RT;
								++instSetCounters[STORE_REGD_AN_PY_SN_UN_WY_SZ4]; return STORE_REGD_AN_PY_SN_UN_WY_SZ4;

							case 0x1:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.SReg[2] = RT;
								++instSetCounters[STORE_REGD_AN_PY_SN_UN_WY_SZ4]; return STORE_REGD_AN_PY_SN_UN_WY_SZ4;

							case 0x2:
								instReg.SReg[0] = RN;
								instReg.SReg[2] = RT;
								++instSetCounters[STORE_IMMD_AN_PY_SN_UN_WY_SZ4]; return STORE_IMMD_AN_PY_SN_UN_WY_SZ4;

							case 0x3:
								instReg.SReg[0] = RN;
								instReg.SReg[2] = RT;
								++instSetCounters[STORE_IMMD_AN_PY_SN_UN_WY_SZ4]; return STORE_IMMD_AN_PY_SN_UN_WY_SZ4;

							case 0x4:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.SReg[2] = RT;
								++instSetCounters[STORE_REGD_AY_PY_SN_UN_WY_SZ4]; return STORE_REGD_AY_PY_SN_UN_WY_SZ4;

							case 0x5:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.SReg[2] = RT;
								++instSetCounters[STORE_REGD_AY_PY_SN_UN_WY_SZ4]; return STORE_REGD_AY_PY_SN_UN_WY_SZ4;

							case 0x6:
								instReg.SReg[0] = RN;
								instReg.SReg[2] = RT;
								++instSetCounters[STORE_IMMD_AY_PY_SN_UN_WY_SZ4]; return STORE_IMMD_AY_PY_SN_UN_WY_SZ4;

							case 0x7:
								instReg.SReg[0] = RN;
								instReg.SReg[2] = RT;
								++instSetCounters[STORE_IMMD_AY_PY_SN_UN_WY_SZ4]; return STORE_IMMD_AY_PY_SN_UN_WY_SZ4;

							case 0x8:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.SReg[2] = RT;
								++instSetCounters[STORE_REGD_AN_PN_SN_UN_WN_SZ4]; return STORE_REGD_AN_PN_SN_UN_WN_SZ4;

							case 0x9:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.SReg[2] = RT;
								++instSetCounters[STORE_REGD_AN_PN_SN_UN_WY_SZ4]; return STORE_REGD_AN_PN_SN_UN_WY_SZ4;

							case 0xa:
								instReg.SReg[0] = RN;
								instReg.SReg[2] = RT;
								++instSetCounters[STORE_IMMD_AN_PN_SN_UN_WN_SZ4]; return STORE_IMMD_AN_PN_SN_UN_WN_SZ4;

							case 0xb:
								instReg.SReg[0] = RN;
								instReg.SReg[2] = RT;
								++instSetCounters[STORE_IMMD_AN_PN_SN_UN_WY_SZ4]; return STORE_IMMD_AN_PN_SN_UN_WY_SZ4;

							case 0xc:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.SReg[2] = RT;
								++instSetCounters[STORE_REGD_AY_PN_SN_UN_WN_SZ4]; return STORE_REGD_AY_PN_SN_UN_WN_SZ4;

							case 0xd:
								instReg.SReg[0] = RN;
								instReg.SReg[1] = RM;
								instReg.SReg[2] = RT;
								++instSetCounters[STORE_REGD_AY_PN_SN_UN_WY_SZ4]; return STORE_REGD_AY_PN_SN_UN_WY_SZ4;

							case 0xe:
								instReg.SReg[0] = RN;
								instReg.SReg[2] = RT;
								++instSetCounters[STORE_IMMD_AY_PN_SN_UN_WN_SZ4]; return STORE_IMMD_AY_PN_SN_UN_WN_SZ4;

							case 0xf:
								instReg.SReg[0] = RN;
								instReg.SReg[2] = RT;
								++instSetCounters[STORE_IMMD_AY_PN_SN_UN_WY_SZ4]; return STORE_IMMD_AY_PN_SN_UN_WY_SZ4;

							default:
								++instSetCounters[Unknown32]; return Unknown32;
							}

						}
					default:
						++instSetCounters[Unknown32]; return Unknown32;
					}
				}
				break;
				default:
					// Unknown32::unknown(([], {}))
					++instSetCounters[Unknown32]; return Unknown32;
					break;
				}

			case 0x0:
				switch (IS_MISC) {
				case 0x0:
					// ArmDataProcReg::armDataProcReg(([], {}))
				{
					const bool immShift = (bits(machInst, 4) == 0);
					const bool setCc = (bits(machInst, 20) == 1);
					const uint32_t imm5 = bits(machInst, 11, 7);
					const ArmShiftType type = (ArmShiftType)(uint32_t)bits(machInst, 6, 5);
					const IntRegIndex rd = (IntRegIndex)(uint32_t)RD;
					const IntRegIndex rn = (IntRegIndex)(uint32_t)RN;
					const IntRegIndex rm = (IntRegIndex)(uint32_t)RM;
					const IntRegIndex rs = (IntRegIndex)(uint32_t)RS;
					instReg.SReg[0] = rn;
					switch (OPCODE) {
					case 0x0:
						instReg.DReg[0] = rd;
						instReg.SReg[1] = rm;
						if (immShift) {
							if (setCc) {
								instType.isCc = true;
								if (rd == INTREG_PC) {

									++instSetCounters[AndsRegPclr]; return AndsRegPclr;

								}
								else {
									++instSetCounters[AndRegCc]; return AndRegCc;
								}
							}
							else {
								++instSetCounters[AndReg]; return AndReg;
							}
						}
						else {
							instReg.SReg[2] = rs;
							if (setCc) {
								instType.isCc = true;
								++instSetCounters[AndRegCc]; return AndRegCc;
							}
							else {
								++instSetCounters[AndReg]; return AndReg;
							}
						}
						break;

					case 0x1:
						instReg.DReg[0] = rd;
						instReg.SReg[1] = rm;
						if (immShift) {
							if (setCc) {
								instType.isCc = true;
								if (rd == INTREG_PC) {
									++instSetCounters[EorsRegPclr]; return EorsRegPclr;
								}
								else {
									++instSetCounters[EorRegCc]; return EorRegCc;
								}
							}
							else {
								++instSetCounters[EorReg]; return EorReg;
							}
						}
						else {
							instReg.SReg[2] = rs;
							if (setCc) {
								instType.isCc = true;
								++instSetCounters[EorRegCc]; return EorRegCc;
							}
							else {
								++instSetCounters[EorReg]; return EorReg;
							}
						}
						break;

					case 0x2:
						instReg.DReg[0] = rd;
						instReg.SReg[1] = rm;
						if (immShift) {
							if (setCc) {
								instType.isCc = true;
								if (rd == INTREG_PC) {

									++instSetCounters[SubsRegPclr];
									return SubsRegPclr;
								}
								else {
									++instSetCounters[SubRegCc];
									return SubRegCc;
								}
							} else {
								++instSetCounters[SubReg];
								return SubReg;
							}
						}else {
							instReg.SReg[2] = rs;
							if (setCc) {
								instType.isCc = true;
								++instSetCounters[SubRegCc]; return SubRegCc;
							}
							else {
								++instSetCounters[SubReg]; return SubReg;
							}
						}
						break;

					case 0x3:
						instReg.DReg[0] = rd;
						instReg.SReg[1] = rm;
						if (immShift) {
							if (setCc) {
								instType.isCc = true;
								if (rd == INTREG_PC) {

									++instSetCounters[RsbsRegPclr]; return RsbsRegPclr;

								}
								else {
									++instSetCounters[RsbRegCc]; return RsbRegCc;
								}
							}
							else {
								++instSetCounters[RsbReg]; return RsbReg;
							}
						}
						else {
							instReg.SReg[2] = rs;
							if (setCc) {
								instType.isCc = true;
								++instSetCounters[RsbRegCc]; return RsbRegCc;
							}
							else {
								++instSetCounters[RsbReg]; return RsbReg;
							}
						}
						break;

					case 0x4:
						instReg.DReg[0] = rd;
						instReg.SReg[1] = rm;
						if (immShift) {
							if (setCc) {
								instType.isCc = true;
								if (rd == INTREG_PC) {

									++instSetCounters[AddsRegPclr]; return AddsRegPclr;

								}
								else {
									++instSetCounters[AddRegCc]; return AddRegCc;
								}
							}
							else {
								++instSetCounters[AddReg]; return AddReg;
							}
						}
						else {
							instReg.SReg[2] = rs;
							if (setCc) {
								instType.isCc = true;
								++instSetCounters[AddRegCc]; return AddRegCc;
							}
							else {
								++instSetCounters[AddReg]; return AddReg;
							}
						}
						break;

					case 0x5:
						instReg.DReg[0] = rd;
						instReg.SReg[1] = rm;
						if (immShift) {
							if (setCc) {
								instType.isCc = true;
								if (rd == INTREG_PC) {
									++instSetCounters[AdcsRegPclr]; return AdcsRegPclr;
								}
								else {
									++instSetCounters[AdcRegCc]; return AdcRegCc;
								}
							}
							else {
								++instSetCounters[AdcReg]; return AdcReg;
							}
						}
						else {
							instReg.SReg[2] = rs;
							if (setCc) {
								instType.isCc = true;
								++instSetCounters[AdcRegCc]; return AdcRegCc;
							}
							else {
								++instSetCounters[AdcReg]; return AdcReg;
							}
						}
						break;

					case 0x6:
						instReg.DReg[0] = rd;
						instReg.SReg[1] = rm;
						if (immShift) {
							if (setCc) {
								instType.isCc = true;
								if (rd == INTREG_PC) {
									++instSetCounters[SbcsRegPclr]; return SbcsRegPclr;
								}
								else {
									++instSetCounters[SbcRegCc]; return SbcRegCc;
								}
							}
							else {
								++instSetCounters[SbcReg]; return SbcReg;
							}
						}
						else {
							instReg.SReg[2] = rs;
							if (setCc) {
								instType.isCc = true;
								++instSetCounters[SbcRegCc]; return SbcRegCc;
							}
							else {
								++instSetCounters[SbcReg]; return SbcReg;
							}
						}
						break;

					case 0x7:
						instReg.DReg[0] = rd;
						instReg.SReg[1] = rm;
						if (immShift) {
							if (setCc) {
								instType.isCc = true;
								if (rd == INTREG_PC) {
									++instSetCounters[RscsRegPclr]; return RscsRegPclr;
								}
								else {
									++instSetCounters[RscRegCc]; return RscRegCc;
								}
							}
							else {
								++instSetCounters[RscReg]; return RscReg;
							}
						}
						else {
							instReg.SReg[2] = rs;
							if (setCc) {
								instType.isCc = true;
								++instSetCounters[RscRegCc]; return RscRegCc;
							}
							else {
								++instSetCounters[RscReg]; return RscReg;
							}
						}
						break;

					case 0x8:
						instReg.SReg[1] = rm;
						if (immShift) {
							if (setCc) {
								instType.isCc = true;
								if (INTREG_ZERO == INTREG_PC) {

								}
								else {
									++instSetCounters[TstRegCc]; return TstRegCc;
								}
							}
							else {
								++instSetCounters[TstReg]; return TstReg;
							}
						}
						else {
							instReg.SReg[2] = rs;
							if (setCc) {
								instType.isCc = true;
								++instSetCounters[TstRegCc]; return TstRegCc;
							}
							else {
								++instSetCounters[TstReg]; return TstReg;
							}
						}
						break;

					case 0x9:
						instReg.SReg[1] = rm;
						if (immShift) {
							if (setCc) {
								instType.isCc = true;
								if (INTREG_ZERO == INTREG_PC) {

								}
								else {
									++instSetCounters[TeqRegCc]; return TeqRegCc;
								}
							}
							else {
								++instSetCounters[TeqReg]; return TeqReg;
							}
						}
						else {
							instReg.SReg[2] = rs;
							if (setCc) {
								instType.isCc = true;
								++instSetCounters[TeqRegCc]; return TeqRegCc;
							}
							else {
								++instSetCounters[TeqReg]; return TeqReg;
							}
						}
						break;

					case 0xa:
						instReg.SReg[1] = rm;
						if (immShift) {
							if (setCc) {
								instType.isCc = true;
								if (INTREG_ZERO == INTREG_PC) {

								}
								else {
									++instSetCounters[CmpRegCc]; return CmpRegCc;
								}
							}
							else {
								++instSetCounters[CmpReg]; return CmpReg;
							}
						}
						else {
							instReg.SReg[2] = rs;
							if (setCc) {
								instType.isCc = true;
								++instSetCounters[CmpRegCc]; return CmpRegCc;
							}
							else {
								++instSetCounters[CmpReg]; return CmpReg;
							}
						}
						break;

					case 0xb:
						instReg.SReg[1] = rm;
						if (immShift) {
							if (setCc) {
								instType.isCc = true;
								if (INTREG_ZERO == INTREG_PC) {

								}
								else {
									++instSetCounters[CmnRegCc]; return CmnRegCc;
								}
							}
							else {
								++instSetCounters[CmnReg]; return CmnReg;
							}
						}
						else {
							instReg.SReg[2] = rs;
							if (setCc) {
								instType.isCc = true;
								++instSetCounters[CmnRegCc]; return CmnRegCc;
							}
							else {
								++instSetCounters[CmnReg]; return CmnReg;
							}
						}
						break;

					case 0xc:
						instReg.SReg[1] = rm;
						instReg.DReg[0] = rd;
						if (immShift) {
							if (setCc) {
								instType.isCc = true;
								if (rd == INTREG_PC) {

									++instSetCounters[OrrsRegPclr]; return OrrsRegPclr;

								}
								else {
									++instSetCounters[OrrRegCc]; return OrrRegCc;
								}
							}
							else {
								++instSetCounters[OrrReg]; return OrrReg;
							}
						}
						else {
							instReg.SReg[2] = rs;
							if (setCc) {
								instType.isCc = true;
								++instSetCounters[OrrRegCc]; return OrrRegCc;
							}
							else {
								++instSetCounters[OrrReg]; return OrrReg;
							}
						}
						break;

					case 0xd:
						instReg.DReg[0] = rd;
						instReg.SReg[1] = rm;
						if (immShift) {
							if (setCc) {
								instType.isCc = true;
								if (rd == INTREG_PC) {
									++instSetCounters[MovsRegPclr]; return MovsRegPclr;
								}
								else {
									++instSetCounters[MovRegCc]; return MovRegCc;
								}
							}
							else {
								++instSetCounters[MovReg]; return MovReg;
							}
						}
						else {
							instReg.SReg[2] = rs;
							if (setCc) {
								instType.isCc = true;
								++instSetCounters[MovRegCc]; return MovRegCc;
							}
							else {
								++instSetCounters[MovReg]; return MovReg;
							}
						}
						break;

					case 0xe:
						instReg.DReg[0] = rd;
						instReg.SReg[1] = rm;
						if (immShift) {
							if (setCc) {
								instType.isCc = true;
								if (rd == INTREG_PC) {
									++instSetCounters[BicsRegPclr]; return BicsRegPclr;
								}
								else {
									++instSetCounters[BicRegCc]; return BicRegCc;
								}
							}
							else {
								++instSetCounters[BicReg]; return BicReg;
							}
						}
						else {
							instReg.SReg[2] = rs;
							if (setCc) {
								instType.isCc = true;
								++instSetCounters[BicRegCc]; return BicRegCc;
							}
							else {
								++instSetCounters[BicReg]; return BicReg;
							}
						}
						break;

					case 0xf:
						instReg.SReg[1] = rm;
						instReg.DReg[0] = rd;
						if (immShift) {
							if (setCc) {
								instType.isCc = true;
								if (rd == INTREG_PC) {
									++instSetCounters[MvnsRegPclr]; return MvnsRegPclr;
								}
								else {
									++instSetCounters[MvnRegCc]; return MvnRegCc;
								}
							}
							else {
								++instSetCounters[MvnReg]; return MvnReg;
							}
						}
						else {
							instReg.SReg[2] = rs;
							if (setCc) {
								instType.isCc = true;
								++instSetCounters[MvnRegCc]; return MvnRegCc;
							}
							else {
								++instSetCounters[MvnReg]; return MvnReg;
							}
						}
						break;

					default:
						++instSetCounters[Unknown32]; return Unknown32;
							}
						}
						break;

					case 0x1:
						switch (OPCODE_7) {
						case 0x0:
							switch (MISC_OPCODE) {
							case 0x0:
								// ArmMsrMrs::armMsrMrs(([], {}))
							{
								const uint8_t byteMask = bits(machInst, 19, 16);
								const uint8_t sysM = byteMask | (bits(machInst, 8) << 4);
								const IntRegIndex rn = (IntRegIndex)(uint32_t)bits(machInst, 3, 0);
								const IntRegIndex rd = (IntRegIndex)(uint32_t)bits(machInst, 15, 12);
								const uint32_t opcode = bits(machInst, 24, 21);
								const bool useImm = bits(machInst, 25);
								const bool r = bits(machInst, 22);
								const bool isBanked = bits(machInst, 9);
								const uint32_t unrotated = bits(machInst, 7, 0);
								const uint32_t rotation = (bits(machInst, 11, 8) << 1);
								switch (opcode) { // pstate bit
								case 0x8:
									instReg.DReg[0] = rd;
									if (isBanked) {
										++instSetCounters[MrsBankedReg]; return MrsBankedReg;
									}
									else {
										++instSetCounters[MrsCpsr]; return MrsCpsr;
									}
								case 0x9:
									instType.isCc = true;
									if (useImm) {
										++instSetCounters[MsrCpsrImm]; return MsrCpsrImm;
									}
									else {
										instReg.SReg[0] = rn;
										if (isBanked) {
											++instSetCounters[MsrBankedReg]; return MsrBankedReg;
										}
										else {
											++instSetCounters[MsrCpsrReg]; return MsrCpsrReg;
										}
									}
								case 0xa:
									instReg.DReg[0] = rd;
									if (isBanked) {
										++instSetCounters[MrsBankedReg]; return MrsBankedReg;
									}
									else {
										++instSetCounters[MrsSpsr]; return MrsSpsr;
									}
								case 0xb:
									instType.isCc = true;
									if (useImm) {
										++instSetCounters[MsrSpsrImm]; return MsrSpsrImm;
									}
									else {
										instReg.SReg[0] = rn;
										if (isBanked) {
											++instSetCounters[MsrBankedReg]; return MsrBankedReg;
										}
										else {
											++instSetCounters[MsrSpsrReg]; return MsrSpsrReg;
										}
									}
								default:
									++instSetCounters[Unknown32]; return Unknown32;
								}
							}
							break;

							case 0x1: case 0x2:
								// ArmBxClz::armBxClz(([], {}))

							{
								const IntRegIndex rm = (IntRegIndex)(uint32_t)bits(machInst, 3, 0);
								const IntRegIndex rd = (IntRegIndex)(uint32_t)bits(machInst, 15, 12);
								instReg.SReg[1] = rm;
								if (OPCODE == 0x9) {
									instType.isBranch = true;
									++instSetCounters[BxReg]; return BxReg;
								}
								else if (OPCODE == 0xb) {
									instReg.DReg[0] = rd;
									++instSetCounters[Clz]; return Clz;
								}
								else {
									++instSetCounters[Unknown32]; return Unknown32;
								}
							}
							break;

							case 0x3:
							{
								const IntRegIndex rm = (IntRegIndex)(uint32_t)bits(machInst, 3, 0);
								switch (OPCODE) {
								case 0x9:
									// ArmBlxReg::armBlxReg(([], {}))
									instReg.SReg[1] = rm;
									instType.isBranch = true;
									++instSetCounters[BlxReg]; return BlxReg;
									break;

								default:
									// Unknown32::unknown(([], {}))
									++instSetCounters[Unknown32]; return Unknown32;
									break;
								}
							}
							case 0x5:
								// ArmSatAddSub::armSatAddSub(([], {}))
							{
								IntRegIndex rn = (IntRegIndex)(uint32_t)bits(machInst, 19, 16);
								IntRegIndex rd = (IntRegIndex)(uint32_t)bits(machInst, 15, 12);
								IntRegIndex rm = (IntRegIndex)(uint32_t)bits(machInst, 3, 0);
								instReg.DReg[0] = rd;
								instReg.SReg[0] = rn;
								instReg.SReg[1] = rm;
								instType.isCc = true;
								switch (OPCODE) {
								case 0x8:
									++instSetCounters[QaddRegCc]; return QaddRegCc;
								case 0x9:
									++instSetCounters[QsubRegCc]; return QsubRegCc;
								case 0xa:
									++instSetCounters[QdaddRegCc]; return QdaddRegCc;
								case 0xb:
									++instSetCounters[QdsubRegCc]; return QdsubRegCc;
								default:
									++instSetCounters[Unknown32]; return Unknown32;
								}
							}
							break;

							case 0x6:
							{
								// ArmERet::armERet(([], {}))
								instType.isCc = true;
								++instSetCounters[Eret]; return Eret;
							}

							case 0x7:
							{
								switch (OPCODE_22) {
								case 0x0:
									// Breakpoint::bkpt(([], {}))
									++instSetCounters[Breakpoint]; return Breakpoint;
									break;
								case 0x1:
									// ArmSmcHyp::armSmcHyp(([], {}))

								{
									if (bits(machInst, 21))
									{
										++instSetCounters[Smc]; return Smc;
									}
									else {
										uint32_t imm16 = (bits(machInst, 19, 8) << 4) |
											(bits(machInst, 3, 0) << 0);
										++instSetCounters[Hvc]; return Hvc;
									}
								}
								break;

								default:

									// Unknown32::unknown(([], {}))
									++instSetCounters[Unknown32]; return Unknown32;
									break;
								}
							}
							default:
							{
								// Unknown32::unknown(([], {}))
								++instSetCounters[Unknown32]; return Unknown32;
								break;
							}
							}

						case 0x1:
							// ArmHalfWordMultAndMultAcc::armHalfWordMultAndMultAcc(([], {}))

						{
							const uint32_t op1 = bits(machInst, 22, 21);
							const bool op = bits(machInst, 5);
							const IntRegIndex rd = (IntRegIndex)(uint32_t)bits(machInst, 19, 16);
							const IntRegIndex ra = (IntRegIndex)(uint32_t)bits(machInst, 15, 12);
							const IntRegIndex rm = (IntRegIndex)(uint32_t)bits(machInst, 11, 8);
							const IntRegIndex rn = (IntRegIndex)(uint32_t)bits(machInst, 3, 0);
							instReg.DReg[0] = rd;
							instReg.SReg[0] = rn;
							instReg.SReg[1] = rm;
							instReg.SReg[2] = ra;
							switch (op1) {
							case 0x0:
								switch (bits(machInst, 6, 5)) {
									instType.isCc = true;
								case 0x0:
									++instSetCounters[SmlabbCc]; return SmlabbCc;
								case 0x1:
									++instSetCounters[SmlatbCc]; return SmlatbCc;
								case 0x2:
									++instSetCounters[SmlabtCc]; return SmlabtCc;
								case 0x3:
									++instSetCounters[SmlattCc]; return SmlattCc;
								}
							case 0x1:
								if (op) {
									if (bits(machInst, 6)) {
										++instSetCounters[Smulwt]; return Smulwt;
									}
									else {
										++instSetCounters[Smulwb]; return Smulwb;
									}
								}
								else {
									instType.isCc = true;
									if (bits(machInst, 6)) {
										++instSetCounters[SmlawtCc]; return SmlawtCc;
									}
									else {
										++instSetCounters[SmlawbCc]; return SmlawbCc;
									}
								}
							case 0x2:
								switch (bits(machInst, 6, 5)) {
								case 0x0:
									++instSetCounters[Smlal]; return Smlal;
								case 0x1:
									++instSetCounters[Smlal]; return Smlal;
								case 0x2:
									++instSetCounters[Smlal]; return Smlal;
								case 0x3:
									++instSetCounters[Smlal]; return Smlal;
								}
							case 0x3:
								switch (bits(machInst, 6, 5)) {
								case 0x0:
									++instSetCounters[Smulbb]; return Smulbb;
								case 0x1:
									++instSetCounters[Smultb]; return Smultb;
								case 0x2:
									++instSetCounters[Smulbt]; return Smulbt;
								case 0x3:
									++instSetCounters[Smultt]; return Smultt;
								}
							}
						}
						break;

						default:

							// Unknown32::unknown(([], {}))
							++instSetCounters[Unknown32]; return Unknown32;
							break;
						}

					default:

						// Unknown32::unknown(([], {}))
						++instSetCounters[Unknown32]; return Unknown32;
						break;
					}

				default:

					// Unknown32::unknown(([], {}))
					++instSetCounters[Unknown32]; return Unknown32;
					break;
				}

				case 0x1:
					switch (IS_MISC) {

					case 0x0:
						// ArmDataProcImm::armDataProcImm(([], {}))

					{
						const bool setCc = (bits(machInst, 20) == 1);
						const uint32_t unrotated = bits(machInst, 7, 0);
						const uint32_t rotation = (bits(machInst, 11, 8) << 1);
						const bool rotC = (rotation != 0);
						//const uint32_t imm = rotate_imm(unrotated, rotation);
						const IntRegIndex rd = (IntRegIndex)(uint32_t)RD;
						const IntRegIndex rn = (IntRegIndex)(uint32_t)RN;
						switch (OPCODE) {

						case 0x0:
							instReg.DReg[0] = rd;
							instReg.SReg[0] = rn;
							if (setCc) {
								instType.isCc = true;
								if (true && rd == INTREG_PC) {
									++instSetCounters[AndsImmPclr]; return AndsImmPclr;
								}
								else {
									++instSetCounters[AndImmCc]; return AndImmCc;
								}
							}
							else {
								if (false && rn == INTREG_PC) {

								}
								else {
									++instSetCounters[AndImm]; return AndImm;
								}
							}
							break;

						case 0x1:
							instReg.DReg[0] = rd;
							instReg.SReg[0] = rn;
							if (setCc) {
								instType.isCc = true;
								if (true && rd == INTREG_PC) {

									++instSetCounters[EorsImmPclr]; return EorsImmPclr;

								}
								else {
									++instSetCounters[EorImmCc]; return EorImmCc;
								}
							}
							else {
								if (false && rn == INTREG_PC) {

								}
								else {
									++instSetCounters[EorImm]; return EorImm;
								}
							}
							break;

						case 0x2:
							instReg.DReg[0] = rd;
							instReg.SReg[0] = rn;
							if (setCc) {
								instType.isCc = true;
								if (true && rd == INTREG_PC) {

									++instSetCounters[SubsImmPclr]; return SubsImmPclr;

								}
								else {
									++instSetCounters[SubImmCc]; return SubImmCc;
								}
							}
							else {
								if (true && rn == INTREG_PC) {

									++instSetCounters[AdrImm]; return AdrImm;

								}
								else {
									++instSetCounters[SubImm]; return SubImm;
								}
							}
							break;

						case 0x3:
							instReg.DReg[0] = rd;
							instReg.SReg[0] = rn;
							if (setCc) {
								instType.isCc = true;
								if (true && rd == INTREG_PC) {
									++instSetCounters[RsbsImmPclr]; return RsbsImmPclr;
								}
								else {
									++instSetCounters[RsbImmCc]; return RsbImmCc;
								}
							}
							else {
								if (false && rn == INTREG_PC) {

								}
								else {
									++instSetCounters[RsbImm]; return RsbImm;
								}
							}
							break;

						case 0x4:
							instReg.DReg[0] = rd;
							instReg.SReg[0] = rn;
							if (setCc) {
								instType.isCc = true;
								if (true && rd == INTREG_PC) {

									++instSetCounters[AddsImmPclr]; return AddsImmPclr;

								}
								else {
									++instSetCounters[AddImmCc]; return AddImmCc;
								}
							}
							else {
								if (true && rn == INTREG_PC) {

									++instSetCounters[AdrImm]; return AdrImm;

								}
								else {
									++instSetCounters[AddImm]; return AddImm;
								}
							}
							break;

						case 0x5:
							instReg.DReg[0] = rd;
							instReg.SReg[0] = rn;
							if (setCc) {
								instType.isCc = true;
								if (true && rd == INTREG_PC) {

									++instSetCounters[AdcsImmPclr]; return AdcsImmPclr;

								}
								else {
									++instSetCounters[AdcImmCc]; return AdcImmCc;
								}
							}
							else {
								if (false && rn == INTREG_PC) {

								}
								else {
									++instSetCounters[AdcImm]; return AdcImm;
								}
							}
							break;

						case 0x6:
							instReg.DReg[0] = rd;
							instReg.SReg[0] = rn;
							if (setCc) {
								instType.isCc = true;
								if (true && rd == INTREG_PC) {

									++instSetCounters[SbcsImmPclr]; return SbcsImmPclr;

								}
								else {
									++instSetCounters[SbcImmCc]; return SbcImmCc;
								}
							}
							else {
								if (false && rn == INTREG_PC) {

								}
								else {
									++instSetCounters[SbcImm]; return SbcImm;
								}
							}
							break;

						case 0x7:
							instReg.DReg[0] = rd;
							instReg.SReg[0] = rn;
							if (setCc) {
								instType.isCc = true;
								if (true && rd == INTREG_PC) {

									++instSetCounters[RscsImmPclr]; return RscsImmPclr;

								}
								else {
									++instSetCounters[RscImmCc]; return RscImmCc;
								}
							}
							else {
								if (false && rn == INTREG_PC) {

								}
								else {
									++instSetCounters[RscImm]; return RscImm;
								}
							}
							break;

						case 0x8:
							instReg.SReg[0] = rn;
							if (setCc) {
								instType.isCc = true;
								if (false && INTREG_ZERO == INTREG_PC) {

								}
								else {
									++instSetCounters[TstImmCc]; return TstImmCc;
								}
							}
							else {
								if (false && rn == INTREG_PC) {

								}
								else {
									++instSetCounters[TstImm]; return TstImm;
								}
							}
							break;

						case 0x9:
							instReg.SReg[0] = rn;
							if (setCc) {
								instType.isCc = true;
								if (false && INTREG_ZERO == INTREG_PC) {

								}
								else {
									++instSetCounters[TeqImmCc]; return TeqImmCc;
								}
							}
							else {
								if (false && rn == INTREG_PC) {

								}
								else {
									++instSetCounters[TeqImm]; return TeqImm;
								}
							}
							break;

						case 0xa:
							instReg.SReg[0] = rn;
							if (setCc) {
								instType.isCc = true;
								if (false && INTREG_ZERO == INTREG_PC) {

								}
								else {
									++instSetCounters[CmpImmCc]; return CmpImmCc;
								}
							}
							else {
								if (false && rn == INTREG_PC) {

								}
								else {
									++instSetCounters[CmpImm]; return CmpImm;
								}
							}
							break;

						case 0xb:
							instReg.SReg[0] = rn;
							if (setCc) {
								instType.isCc = true;
								if (false && INTREG_ZERO == INTREG_PC) {

								}
								else {
									++instSetCounters[CmnImmCc]; return CmnImmCc;
								}
							}
							else {
								if (false && rn == INTREG_PC) {

								}
								else {
									++instSetCounters[CmnImm]; return CmnImm;
								}
							}
							break;

						case 0xc:
							instReg.DReg[0] = rd;
							instReg.SReg[0] = rn;
							if (setCc) {
								instType.isCc = true;
								if (true && rd == INTREG_PC) {

									++instSetCounters[OrrsImmPclr]; return OrrsImmPclr;

								}
								else {
									++instSetCounters[OrrImmCc]; return OrrImmCc;
								}
							}
							else {
								if (false && rn == INTREG_PC) {

								}
								else {
									++instSetCounters[OrrImm]; return OrrImm;
								}
							}
							break;

						case 0xd:
							instReg.DReg[0] = rd;
							if (setCc) {
								instType.isCc = true;
								if (true && rd == INTREG_PC) {

									++instSetCounters[MovsImmPclr]; return MovsImmPclr;

								}
								else {
									++instSetCounters[MovImmCc]; return MovImmCc;
								}
							}
							else {
								if (false && INTREG_ZERO == INTREG_PC) {

								}
								else {
									++instSetCounters[MovImm]; return MovImm;
								}
							}
							break;

						case 0xe:
							if (setCc) {
								instType.isCc = true;
								if (true && rd == INTREG_PC) {

									++instSetCounters[BicsImmPclr]; return BicsImmPclr;

								}
								else {
									++instSetCounters[BicImmCc]; return BicImmCc;
								}
							}
							else {
								if (false && rn == INTREG_PC) {

								}
								else {
									++instSetCounters[BicImm]; return BicImm;
								}
							}
							break;

						case 0xf:
							instReg.DReg[0] = rd;
							instReg.SReg[0] = rn;
							if (setCc) {
								instType.isCc = true;
								if (true && rd == INTREG_PC) {

									++instSetCounters[MvnsImmPclr]; return MvnsImmPclr;

								}
								else {
									++instSetCounters[MvnImmCc]; return MvnImmCc;
								}
							}
							else {
								if (false && INTREG_ZERO == INTREG_PC) {

								}
								else {
									++instSetCounters[MvnImm]; return MvnImm;
								}
							}
							break;

						default:
							++instSetCounters[Unknown32]; return Unknown32;
						}
					}
					break;
					case 0x1:
						// ArmMisc::armMisc(([], {}))
					{
						const uint32_t unrotated = bits(machInst, 7, 0);
						const uint32_t rotation = (bits(machInst, 11, 8) << 1);
						//const uint32_t imm = rotate_imm(unrotated, rotation);
						const uint8_t byteMask = bits(machInst, 19, 16);
						switch (OPCODE) {
						case 0x8:
							++instSetCounters[MovImm]; return MovImm;
						case 0x9:
							if (RN == 0) {
								switch (IMM) {
								case 0x0:
									++instSetCounters[NopInst]; return NopInst;
								case 0x1:
									++instSetCounters[YieldInst]; return YieldInst;
								case 0x2:
									++instSetCounters[WfeInst]; return WfeInst;
								case 0x3:
									++instSetCounters[WfiInst]; return WfiInst;
								case 0x4:
									++instSetCounters[SevInst]; return SevInst;
								default:
									++instSetCounters[Unknown32]; return Unknown32;
								}
							}
							else {
								instType.isCc = true;
								++instSetCounters[MsrCpsrImm]; return MsrCpsrImm;
							}
						case 0xa:
						{
							const uint32_t timm = (bits(machInst, 19, 16) << 12) |
								bits(machInst, 11, 0);
							instReg.DReg[0] = RD;
							++instSetCounters[MovtImm]; return MovtImm;
						}
						case 0xb:
							instType.isCc = true;
							++instSetCounters[MsrSpsrImm]; return MsrSpsrImm;
						default:
							++instSetCounters[Unknown32]; return Unknown32;
						}
					}
					break;

					default:

						// Unknown32::unknown(([], {}))
						++instSetCounters[Unknown32]; return Unknown32;
						break;
					}

				case 0x2:
					// AddrMode2::addrMode2((['True'], {}))
					instReg.SReg[0] = RN;
					switch (PUBWL) {
					case 0x1:
						instType.isLoad = true;
						instReg.DReg[0] = RD;
						++instSetCounters[LOAD_IMM_AN_PY_SN_UN_WY_SZ4]; return LOAD_IMM_AN_PY_SN_UN_WY_SZ4;
						break;

					case 0x0:
						instType.isStore = true;
						instReg.SReg[1] = RD;
						++instSetCounters[STORE_IMM_AN_PY_SN_UN_WY_SZ4]; return STORE_IMM_AN_PY_SN_UN_WY_SZ4;
						break;

					case 0x3:
						instType.isLoad = true;
						instReg.DReg[0] = RD;
						++instSetCounters[LOAD_IMM_AN_PY_SN_UY_WY_SZ4]; return LOAD_IMM_AN_PY_SN_UY_WY_SZ4;
						break;

					case 0x2:
						instType.isStore = true;
						instReg.SReg[1] = RD;
						++instSetCounters[STORE_IMM_AN_PY_SN_UY_WY_SZ4]; return STORE_IMM_AN_PY_SN_UY_WY_SZ4;
						break;

					case 0x5:
						instType.isLoad = true;
						instReg.DReg[0] = RD;
						++instSetCounters[LOAD_IMM_AN_PY_SN_UN_WY_SZ1]; return LOAD_IMM_AN_PY_SN_UN_WY_SZ1;
						break;

					case 0x4:
						instType.isStore = true;
						instReg.SReg[1] = RD;
						++instSetCounters[STORE_IMM_AN_PY_SN_UN_WY_SZ1]; return STORE_IMM_AN_PY_SN_UN_WY_SZ1;
						break;

					case 0x7:
						instType.isLoad = true;
						instReg.DReg[0] = RD;
						++instSetCounters[LOAD_IMM_AN_PY_SN_UY_WY_SZ1]; return LOAD_IMM_AN_PY_SN_UY_WY_SZ1;
						break;

					case 0x6:
						instType.isStore = true;
						instReg.SReg[1] = RD;
						++instSetCounters[STORE_IMM_AN_PY_SN_UY_WY_SZ1]; return STORE_IMM_AN_PY_SN_UY_WY_SZ1;
						break;

					case 0x9:
						instType.isLoad = true;
						instReg.DReg[0] = RD;
						++instSetCounters[LOAD_IMM_AY_PY_SN_UN_WY_SZ4]; return LOAD_IMM_AY_PY_SN_UN_WY_SZ4;
						break;

					case 0x8:
						instType.isStore = true;
						instReg.SReg[1] = RD;
						++instSetCounters[STORE_IMM_AY_PY_SN_UN_WY_SZ4]; return STORE_IMM_AY_PY_SN_UN_WY_SZ4;
						break;

					case 0xb:
						instType.isLoad = true;
						instReg.DReg[0] = RD;
						++instSetCounters[LOAD_IMM_AY_PY_SN_UY_WY_SZ4]; return LOAD_IMM_AY_PY_SN_UY_WY_SZ4;
						break;

					case 0xa:
						instType.isStore = true;
						instReg.SReg[1] = RD;
						++instSetCounters[STORE_IMM_AY_PY_SN_UY_WY_SZ4]; return STORE_IMM_AY_PY_SN_UY_WY_SZ4;
						break;

					case 0xd:
						instType.isLoad = true;
						instReg.DReg[0] = RD;
						++instSetCounters[LOAD_IMM_AY_PY_SN_UN_WY_SZ1]; return LOAD_IMM_AY_PY_SN_UN_WY_SZ1;
						break;

					case 0xc:
						instType.isStore = true;
						instReg.SReg[1] = RD;
						++instSetCounters[STORE_IMM_AY_PY_SN_UN_WY_SZ1]; return STORE_IMM_AY_PY_SN_UN_WY_SZ1;
						break;

					case 0xf:
						instType.isLoad = true;
						instReg.DReg[0] = RD;
						++instSetCounters[LOAD_IMM_AY_PY_SN_UY_WY_SZ1]; return LOAD_IMM_AY_PY_SN_UY_WY_SZ1;
						break;

					case 0xe:
						instType.isStore = true;
						instReg.SReg[1] = RD;
						++instSetCounters[STORE_IMM_AY_PY_SN_UY_WY_SZ1]; return STORE_IMM_AY_PY_SN_UY_WY_SZ1;
						break;

					case 0x11:
						instType.isLoad = true;
						instReg.DReg[0] = RD;
						++instSetCounters[LOAD_IMM_AN_PN_SN_UN_WN_SZ4]; return LOAD_IMM_AN_PN_SN_UN_WN_SZ4;
						break;

					case 0x10:
						instType.isStore = true;
						instReg.SReg[1] = RD;
						++instSetCounters[STORE_IMM_AN_PN_SN_UN_WN_SZ4]; return STORE_IMM_AN_PN_SN_UN_WN_SZ4;
						break;

					case 0x13:
						instType.isLoad = true;
						instReg.DReg[0] = RD;
						++instSetCounters[LOAD_IMM_AN_PN_SN_UN_WY_SZ4]; return LOAD_IMM_AN_PN_SN_UN_WY_SZ4;
						break;

					case 0x12:
						instType.isStore = true;
						instReg.SReg[1] = RD;
						++instSetCounters[STORE_IMM_AN_PN_SN_UN_WY_SZ4]; return STORE_IMM_AN_PN_SN_UN_WY_SZ4;
						break;

					case 0x15:
						instType.isLoad = true;
						instReg.DReg[0] = RD;
						++instSetCounters[LOAD_IMM_AN_PN_SN_UN_WN_SZ1]; return LOAD_IMM_AN_PN_SN_UN_WN_SZ1;
						break;

					case 0x14:
						instType.isStore = true;
						instReg.SReg[1] = RD;
						++instSetCounters[STORE_IMM_AN_PN_SN_UN_WN_SZ1]; return STORE_IMM_AN_PN_SN_UN_WN_SZ1;
						break;

					case 0x17:
						instType.isLoad = true;
						instReg.DReg[0] = RD;
						++instSetCounters[LOAD_IMM_AN_PN_SN_UN_WY_SZ1]; return LOAD_IMM_AN_PN_SN_UN_WY_SZ1;
						break;

					case 0x16:
						instType.isStore = true;
						instReg.SReg[1] = RD;
						++instSetCounters[STORE_IMM_AN_PN_SN_UN_WY_SZ1]; return STORE_IMM_AN_PN_SN_UN_WY_SZ1;
						break;

					case 0x19:
						instType.isLoad = true;
						instReg.DReg[0] = RD;
						++instSetCounters[LOAD_IMM_AY_PN_SN_UN_WN_SZ4]; return LOAD_IMM_AY_PN_SN_UN_WN_SZ4;
						break;

					case 0x18:
						instType.isStore = true;
						instReg.SReg[1] = RD;
						++instSetCounters[STORE_IMM_AY_PN_SN_UN_WN_SZ4]; return STORE_IMM_AY_PN_SN_UN_WN_SZ4;
						break;

					case 0x1b:
						instType.isLoad = true;
						instReg.DReg[0] = RD;
						++instSetCounters[LOAD_IMM_AY_PN_SN_UN_WY_SZ4]; return LOAD_IMM_AY_PN_SN_UN_WY_SZ4;
						break;

					case 0x1a:
						instType.isStore = true;
						instReg.SReg[1] = RD;
						++instSetCounters[STORE_IMM_AY_PN_SN_UN_WY_SZ4]; return STORE_IMM_AY_PN_SN_UN_WY_SZ4;
						break;

					case 0x1d:
						instType.isLoad = true;
						instReg.DReg[0] = RD;
						++instSetCounters[LOAD_IMM_AY_PN_SN_UN_WN_SZ1]; return LOAD_IMM_AY_PN_SN_UN_WN_SZ1;
						break;

					case 0x1c:
						instType.isStore = true;
						instReg.SReg[1] = RD;
						++instSetCounters[STORE_IMM_AY_PN_SN_UN_WN_SZ1]; return STORE_IMM_AY_PN_SN_UN_WN_SZ1;
						break;

					case 0x1f:
						instType.isLoad = true;
						instReg.DReg[0] = RD;
						++instSetCounters[LOAD_IMM_AY_PN_SN_UN_WY_SZ1]; return LOAD_IMM_AY_PN_SN_UN_WY_SZ1;
						break;

					case 0x1e:
						instType.isStore = true;
						instReg.SReg[1] = RD;
						++instSetCounters[STORE_IMM_AY_PN_SN_UN_WY_SZ1]; return STORE_IMM_AY_PN_SN_UN_WY_SZ1;
						break;

					default:
						++instSetCounters[Unknown32]; return Unknown32;
						break;
					}
					break;

				case 0x3:
					switch (OPCODE_4) {

					case 0x0:
						// AddrMode2::addrMode2((['False'], {}))
						instReg.SReg[0] = RN;
						switch (PUBWL) {

						case 0x1:

							instType.isLoad = true;
							instReg.DReg[0] = RD;
							++instSetCounters[LOAD_REG_AN_PY_SN_UN_WY_SZ4]; return LOAD_REG_AN_PY_SN_UN_WY_SZ4;
							break;

						case 0x0:
							instType.isStore = true;
							instReg.SReg[1] = RD;
							++instSetCounters[STORE_REG_AN_PY_SN_UN_WY_SZ4]; return STORE_REG_AN_PY_SN_UN_WY_SZ4;
							break;

						case 0x3:
							instType.isLoad = true;
							instReg.DReg[0] = RD;
							++instSetCounters[LOAD_REG_AN_PY_SN_UY_WY_SZ4]; return LOAD_REG_AN_PY_SN_UY_WY_SZ4;
							break;

						case 0x2:
							instType.isStore = true;
							instReg.SReg[1] = RD;
							++instSetCounters[STORE_REG_AN_PY_SN_UY_WY_SZ4]; return STORE_REG_AN_PY_SN_UY_WY_SZ4;
							break;

						case 0x5:
							instType.isLoad = true;
							instReg.DReg[0] = RD;
							++instSetCounters[LOAD_REG_AN_PY_SN_UN_WY_SZ1]; return LOAD_REG_AN_PY_SN_UN_WY_SZ1;
							break;

						case 0x4:
							instType.isStore = true;
							instReg.SReg[1] = RD;
							++instSetCounters[STORE_REG_AN_PY_SN_UN_WY_SZ1]; return STORE_REG_AN_PY_SN_UN_WY_SZ1;
							break;

						case 0x7:
							instType.isLoad = true;
							instReg.DReg[0] = RD;
							++instSetCounters[LOAD_REG_AN_PY_SN_UY_WY_SZ1]; return LOAD_REG_AN_PY_SN_UY_WY_SZ1;
							break;

						case 0x6:
							instType.isStore = true;
							instReg.SReg[1] = RD;
							++instSetCounters[STORE_REG_AN_PY_SN_UY_WY_SZ1]; return STORE_REG_AN_PY_SN_UY_WY_SZ1;
							break;

						case 0x9:
							instType.isLoad = true;
							instReg.DReg[0] = RD;
							++instSetCounters[LOAD_REG_AY_PY_SN_UN_WY_SZ4]; return LOAD_REG_AY_PY_SN_UN_WY_SZ4;
							break;

						case 0x8:
							instType.isStore = true;
							instReg.SReg[1] = RD;
							++instSetCounters[STORE_REG_AY_PY_SN_UN_WY_SZ4]; return STORE_REG_AY_PY_SN_UN_WY_SZ4;
							break;

						case 0xb:
							instType.isLoad = true;
							instReg.DReg[0] = RD;
							++instSetCounters[LOAD_REG_AY_PY_SN_UY_WY_SZ4]; return LOAD_REG_AY_PY_SN_UY_WY_SZ4;
							break;

						case 0xa:
							instType.isStore = true;
							instReg.SReg[1] = RD;
							++instSetCounters[STORE_REG_AY_PY_SN_UY_WY_SZ4]; return STORE_REG_AY_PY_SN_UY_WY_SZ4;
							break;

						case 0xd:
							instType.isLoad = true;
							instReg.DReg[0] = RD;
							++instSetCounters[LOAD_REG_AY_PY_SN_UN_WY_SZ1]; return LOAD_REG_AY_PY_SN_UN_WY_SZ1;
							break;

						case 0xc:
							instType.isStore = true;
							instReg.SReg[1] = RD;
							++instSetCounters[STORE_REG_AY_PY_SN_UN_WY_SZ1]; return STORE_REG_AY_PY_SN_UN_WY_SZ1;
							break;

						case 0xf:
							instType.isLoad = true;
							instReg.DReg[0] = RD;
							++instSetCounters[LOAD_REG_AY_PY_SN_UY_WY_SZ1]; return LOAD_REG_AY_PY_SN_UY_WY_SZ1;
							break;

						case 0xe:
							instType.isStore = true;
							instReg.SReg[1] = RD;
							++instSetCounters[STORE_REG_AY_PY_SN_UY_WY_SZ1]; return STORE_REG_AY_PY_SN_UY_WY_SZ1;
							break;

						case 0x11:
							instType.isLoad = true;
							instReg.DReg[0] = RD;
							++instSetCounters[LOAD_REG_AN_PN_SN_UN_WN_SZ4]; return LOAD_REG_AN_PN_SN_UN_WN_SZ4;
							break;

						case 0x10:
							instType.isStore = true;
							instReg.SReg[1] = RD;
							++instSetCounters[STORE_REG_AN_PN_SN_UN_WN_SZ4]; return STORE_REG_AN_PN_SN_UN_WN_SZ4;
							break;

						case 0x13:
							instType.isLoad = true;
							instReg.DReg[0] = RD;
							++instSetCounters[LOAD_REG_AN_PN_SN_UN_WY_SZ4]; return LOAD_REG_AN_PN_SN_UN_WY_SZ4;
							break;

						case 0x12:
							instType.isStore = true;
							instReg.SReg[1] = RD;
							++instSetCounters[STORE_REG_AN_PN_SN_UN_WY_SZ4]; return STORE_REG_AN_PN_SN_UN_WY_SZ4;
							break;

						case 0x15:
							instType.isLoad = true;
							instReg.DReg[0] = RD;
							++instSetCounters[LOAD_REG_AN_PN_SN_UN_WN_SZ1]; return LOAD_REG_AN_PN_SN_UN_WN_SZ1;
							break;

						case 0x14:
							instType.isStore = true;
							instReg.SReg[1] = RD;
							++instSetCounters[STORE_REG_AN_PN_SN_UN_WN_SZ1]; return STORE_REG_AN_PN_SN_UN_WN_SZ1;
							break;

						case 0x17:
							instType.isLoad = true;
							instReg.DReg[0] = RD;
							++instSetCounters[LOAD_REG_AN_PN_SN_UN_WY_SZ1]; return LOAD_REG_AN_PN_SN_UN_WY_SZ1;
							break;

						case 0x16:
							instType.isStore = true;
							instReg.SReg[1] = RD;
							++instSetCounters[STORE_REG_AN_PN_SN_UN_WY_SZ1]; return STORE_REG_AN_PN_SN_UN_WY_SZ1;
							break;

						case 0x19:
							instType.isLoad = true;
							instReg.DReg[0] = RD;
							++instSetCounters[LOAD_REG_AY_PN_SN_UN_WN_SZ4]; return LOAD_REG_AY_PN_SN_UN_WN_SZ4;
							break;

						case 0x18:
							instType.isStore = true;
							instReg.SReg[1] = RD;
							++instSetCounters[STORE_REG_AY_PN_SN_UN_WN_SZ4]; return STORE_REG_AY_PN_SN_UN_WN_SZ4;
							break;

						case 0x1b:
							instType.isLoad = true;
							instReg.DReg[0] = RD;
							++instSetCounters[LOAD_REG_AY_PN_SN_UN_WY_SZ4]; return LOAD_REG_AY_PN_SN_UN_WY_SZ4;
							break;

						case 0x1a:
							instType.isStore = true;
							instReg.SReg[1] = RD;
							++instSetCounters[STORE_REG_AY_PN_SN_UN_WY_SZ4]; return STORE_REG_AY_PN_SN_UN_WY_SZ4;
							break;

						case 0x1d:
							instType.isLoad = true;
							instReg.DReg[0] = RD;
							++instSetCounters[LOAD_REG_AY_PN_SN_UN_WN_SZ1]; return LOAD_REG_AY_PN_SN_UN_WN_SZ1;
							break;

						case 0x1c:
							instType.isStore = true;
							instReg.SReg[1] = RD;
							++instSetCounters[STORE_REG_AY_PN_SN_UN_WN_SZ1]; return STORE_REG_AY_PN_SN_UN_WN_SZ1;
							break;

						case 0x1f:
							instType.isLoad = true;
							instReg.DReg[0] = RD;
							++instSetCounters[LOAD_REG_AY_PN_SN_UN_WY_SZ1]; return LOAD_REG_AY_PN_SN_UN_WY_SZ1;
							break;

						case 0x1e:
							instType.isStore = true;
							instReg.SReg[1] = RD;
							++instSetCounters[STORE_REG_AY_PN_SN_UN_WY_SZ1]; return STORE_REG_AY_PN_SN_UN_WY_SZ1;
							break;

						default:
							++instSetCounters[Unknown32]; return Unknown32;
							break;
						}break;

					case 0x1:
						switch (OPCODE_24_23) {
						case 0x0:
							// ArmParallelAddSubtract::armParallelAddSubtract(([], {}))
						{
							const uint32_t op1 = bits(machInst, 21, 20);
							const uint32_t op2 = bits(machInst, 7, 5);
							const IntRegIndex rn = (IntRegIndex)(uint32_t)bits(machInst, 19, 16);
							const IntRegIndex rd = (IntRegIndex)(uint32_t)bits(machInst, 15, 12);
							const IntRegIndex rm = (IntRegIndex)(uint32_t)bits(machInst, 3, 0);
							instReg.DReg[0] = rd;
							instReg.SReg[0] = rn;
							if (bits(machInst, 22) == 0) {
								switch (op1) {
								case 0x1:
									instType.isCc = true;
									switch (op2) {
									case 0x0:
										++instSetCounters[Sadd16RegCc]; return Sadd16RegCc;
									case 0x1:
										++instSetCounters[SasxRegCc]; return SasxRegCc;
									case 0x2:
										++instSetCounters[SsaxRegCc]; return SsaxRegCc;
									case 0x3:
										++instSetCounters[Ssub16RegCc]; return Ssub16RegCc;
									case 0x4:
										++instSetCounters[Sadd8RegCc]; return Sadd8RegCc;
									case 0x7:
										++instSetCounters[Ssub8RegCc]; return Ssub8RegCc;
									}
									break;
								case 0x2:
									switch (op2) {
										instReg.SReg[1] = rm;
									case 0x0:
										++instSetCounters[Qadd16Reg]; return Qadd16Reg;
									case 0x1:
										++instSetCounters[QasxReg]; return QasxReg;
									case 0x2:
										++instSetCounters[QsaxReg]; return QsaxReg;
									case 0x3:
										++instSetCounters[Qsub16Reg]; return Qsub16Reg;
									case 0x4:
										++instSetCounters[Qadd8Reg]; return Qadd8Reg;
									case 0x7:
										++instSetCounters[Qsub8Reg]; return Qsub8Reg;
									}
									break;
								case 0x3:
									instReg.SReg[1] = rm;
									switch (op2) {
									case 0x0:
										++instSetCounters[Shadd16Reg]; return Shadd16Reg;
									case 0x1:
										++instSetCounters[ShasxReg]; return ShasxReg;
									case 0x2:
										++instSetCounters[ShsaxReg]; return ShsaxReg;
									case 0x3:
										++instSetCounters[Shsub16Reg]; return Shsub16Reg;
									case 0x4:
										++instSetCounters[Shadd8Reg]; return Shadd8Reg;
									case 0x7:
										++instSetCounters[Shsub8Reg]; return Shsub8Reg;
									}
									break;
								}
							}
							else {
								switch (op1) {
								case 0x1:
									instReg.SReg[1] = rm;
									instType.isCc = true;
									switch (op2) {
									case 0x0:
										++instSetCounters[Uadd16RegCc]; return Uadd16RegCc;
									case 0x1:
										++instSetCounters[UasxRegCc]; return UasxRegCc;
									case 0x2:
										++instSetCounters[UsaxRegCc]; return UsaxRegCc;
									case 0x3:
										++instSetCounters[Usub16RegCc]; return Usub16RegCc;
									case 0x4:
										++instSetCounters[Uadd8RegCc]; return Uadd8RegCc;
									case 0x7:
										++instSetCounters[Usub8RegCc]; return Usub8RegCc;
									}
									break;
								case 0x2:
									instReg.SReg[1] = rm;
									switch (op2) {
									case 0x0:
										++instSetCounters[Uqadd16Reg]; return Uqadd16Reg;
									case 0x1:
										++instSetCounters[UqasxReg]; return UqasxReg;
									case 0x2:
										++instSetCounters[UqsaxReg]; return UqsaxReg;
									case 0x3:
										++instSetCounters[Uqsub16Reg]; return Uqsub16Reg;
									case 0x4:
										++instSetCounters[Uqadd8Reg]; return Uqadd8Reg;
									case 0x7:
										++instSetCounters[Uqsub8Reg]; return Uqsub8Reg;
									}
									break;
								case 0x3:
									instReg.SReg[1] = rm;
									switch (op2) {
									case 0x0:
										++instSetCounters[Uhadd16Reg]; return Uhadd16Reg;
									case 0x1:
										++instSetCounters[UhasxReg]; return UhasxReg;
									case 0x2:
										++instSetCounters[UhsaxReg]; return UhsaxReg;
									case 0x3:
										++instSetCounters[Uhsub16Reg]; return Uhsub16Reg;
									case 0x4:
										++instSetCounters[Uhadd8Reg]; return Uhadd8Reg;
									case 0x7:
										++instSetCounters[Uhsub8Reg]; return Uhsub8Reg;
									}
									break;
								}
							}
							++instSetCounters[Unknown32]; return Unknown32;
						}
						break;

						case 0x1:
							// ArmPackUnpackSatReverse::armPackUnpackSatReverse(([], {}))

						{
							const uint32_t op1 = bits(machInst, 22, 20);
							const uint32_t a = bits(machInst, 19, 16);
							const uint32_t op2 = bits(machInst, 7, 5);
							if (bits(op2, 0) == 0) {
								const IntRegIndex rn = (IntRegIndex)(uint32_t)bits(machInst, 3, 0);
								const IntRegIndex rd = (IntRegIndex)(uint32_t)bits(machInst, 15, 12);
								const uint32_t satImm = bits(machInst, 20, 16);
								const uint32_t imm = bits(machInst, 11, 7);
								const ArmShiftType type = (ArmShiftType)(uint32_t)bits(machInst, 6, 5);
								instReg.SReg[0] = rn;
								instReg.DReg[0] = rd;
								if (op1 == 0) {
									if (type) {
										++instSetCounters[PkhtbReg]; return PkhtbReg;
									}
									else {
										++instSetCounters[PkhbtReg]; return PkhbtReg;
									}
								}
								else if (bits(op1, 2, 1) == 1) {
									instType.isCc = true;
									++instSetCounters[Ssat]; return Ssat;
								}
								else if (bits(op1, 2, 1) == 3) {
									instType.isCc = true;
									++instSetCounters[Usat]; return Usat;
								}
								++instSetCounters[Unknown32]; return Unknown32;
							}
							switch (op1) {
							case 0x0:
							{
								const IntRegIndex rn = (IntRegIndex)(uint32_t)bits(machInst, 19, 16);
								const IntRegIndex rd = (IntRegIndex)(uint32_t)bits(machInst, 15, 12);
								const IntRegIndex rm = (IntRegIndex)(uint32_t)bits(machInst, 3, 0);
								if (op2 == 0x3) {
									const uint32_t rotation = (uint32_t)bits(machInst, 11, 10) << 3;
									if (a == 0xf) {
										instReg.DReg[0] = rd;
										instReg.SReg[1] = rm;
										++instSetCounters[Sxtb16]; return Sxtb16;
									}
									else {
										instReg.DReg[0] = rd;
										instReg.SReg[0] = rn;
										instReg.SReg[1] = rm;
										++instSetCounters[Sxtab16]; return Sxtab16;
									}
								}
								else if (op2 == 0x5) {
									instReg.DReg[0] = rd;
									instReg.SReg[0] = rn;
									instReg.SReg[1] = rm;
									instType.isCc = true;
									++instSetCounters[Sel]; return Sel;
								}
							}
							break;
							case 0x2:
								if (op2 == 0x1) {
									const IntRegIndex rn = (IntRegIndex)(uint32_t)bits(machInst, 3, 0);
									const IntRegIndex rd = (IntRegIndex)(uint32_t)bits(machInst, 15, 12);
									instReg.DReg[0] = rd;
									instReg.SReg[0] = rn;
									instType.isCc = true;
									const uint32_t satImm = bits(machInst, 20, 16);
									++instSetCounters[Ssat]; return Ssat;
								}
								else if (op2 == 0x3) {
									const IntRegIndex rn = (IntRegIndex)(uint32_t)bits(machInst, 19, 16);
									const IntRegIndex rd = (IntRegIndex)(uint32_t)bits(machInst, 15, 12);
									const IntRegIndex rm = (IntRegIndex)(uint32_t)bits(machInst, 3, 0);
									const uint32_t rotation = (uint32_t)bits(machInst, 11, 10) << 3;
									instReg.DReg[0] = rd;
									instReg.SReg[0] = rn;
									instReg.SReg[1] = rm;
									instType.isCc = true;
									if (a == 0xf) {
										++instSetCounters[Sxtb]; return Sxtb;
									}
									else {
										++instSetCounters[Sxtab]; return Sxtab;
									}
								}
								break;
							case 0x3:
								if (op2 == 0x1) {
									IntRegIndex rd = (IntRegIndex)(uint32_t)bits(machInst, 15, 12);
									IntRegIndex rm = (IntRegIndex)(uint32_t)bits(machInst, 3, 0);
									instReg.DReg[0] = rd;
									instReg.SReg[0] = rm;
									++instSetCounters[Rev]; return Rev;
								}
								else if (op2 == 0x3) {
									const IntRegIndex rn = (IntRegIndex)(uint32_t)bits(machInst, 19, 16);
									const IntRegIndex rd = (IntRegIndex)(uint32_t)bits(machInst, 15, 12);
									const IntRegIndex rm = (IntRegIndex)(uint32_t)bits(machInst, 3, 0);
									const uint32_t rotation = (uint32_t)bits(machInst, 11, 10) << 3;
									instReg.DReg[0] = rd;
									instReg.SReg[0] = rn;
									instReg.SReg[1] = rm;
									if (a == 0xf) {
										++instSetCounters[Sxth]; return Sxth;
									}
									else {
										++instSetCounters[Sxtah]; return Sxtah;
									}
								}
								else if (op2 == 0x5) {
									IntRegIndex rd = (IntRegIndex)(uint32_t)bits(machInst, 15, 12);
									IntRegIndex rm = (IntRegIndex)(uint32_t)bits(machInst, 3, 0);
									instReg.DReg[0] = rd;
									instReg.SReg[1] = rm;
									++instSetCounters[Rev]; return Rev;
								}
								break;
							case 0x4:
								if (op2 == 0x3) {
									const IntRegIndex rn = (IntRegIndex)(uint32_t)bits(machInst, 19, 16);
									const IntRegIndex rd = (IntRegIndex)(uint32_t)bits(machInst, 15, 12);
									const IntRegIndex rm = (IntRegIndex)(uint32_t)bits(machInst, 3, 0);
									instReg.DReg[0] = rd;
									instReg.SReg[1] = rm;
									const uint32_t rotation = (uint32_t)bits(machInst, 11, 10) << 3;
									if (a == 0xf) {
										++instSetCounters[Uxtb16]; return Uxtb16;
									}
									else {
										instReg.SReg[0] = rn;
										++instSetCounters[Uxtab16]; return Uxtab16;
									}
								}
								break;
							case 0x6:
								if (op2 == 0x1) {
									const IntRegIndex rn = (IntRegIndex)(uint32_t)bits(machInst, 3, 0);
									const IntRegIndex rd = (IntRegIndex)(uint32_t)bits(machInst, 15, 12);
									const uint32_t satImm = bits(machInst, 20, 16);
									instReg.DReg[0] = rd;
									instReg.SReg[0] = rn;
									++instSetCounters[Usat]; return Usat;
								}
								else if (op2 == 0x3) {
									const IntRegIndex rn = (IntRegIndex)(uint32_t)bits(machInst, 19, 16);
									const IntRegIndex rd = (IntRegIndex)(uint32_t)bits(machInst, 15, 12);
									const IntRegIndex rm = (IntRegIndex)(uint32_t)bits(machInst, 3, 0);
									const uint32_t rotation = (uint32_t)bits(machInst, 11, 10) << 3;
									instReg.DReg[0] = rd;
									instReg.SReg[1] = rm;
									if (a == 0xf) {
										++instSetCounters[Uxtb]; return Uxtb;
									}
									else {
										instReg.SReg[0] = rn;
										++instSetCounters[Uxtab]; return Uxtab;
									}
								}
								break;
							case 0x7:
							{
								const IntRegIndex rn = (IntRegIndex)(uint32_t)bits(machInst, 19, 16);
								const IntRegIndex rd = (IntRegIndex)(uint32_t)bits(machInst, 15, 12);
								const IntRegIndex rm = (IntRegIndex)(uint32_t)bits(machInst, 3, 0);
								instReg.DReg[0] = rd;
								instReg.SReg[1] = rm;
								if (op2 == 0x1) {
									++instSetCounters[Rbit]; return Rbit;
								}
								else if (op2 == 0x3) {
									const uint32_t rotation =
										(uint32_t)bits(machInst, 11, 10) << 3;
									if (a == 0xf) {
										++instSetCounters[Uxth]; return Uxth;
									}
									else {
										instReg.SReg[0] = rn;
										++instSetCounters[Uxtah]; return Uxtah;
									}
								}
								else if (op2 == 0x5) {
									++instSetCounters[Rev]; return Rev;
								}
							}
							break;
							}
							++instSetCounters[Unknown32]; return Unknown32;
						}
						break;

						case 0x2:
							// ArmSignedMultiplies::armSignedMultiplies(([], {}))

						{
							const uint32_t op1 = bits(machInst, 22, 20);
							// This is 7-5 in the manual, but bit 5 is always ignored.
							const uint32_t op2 = bits(machInst, 7, 6);
							const bool aIsF = (bits(machInst, 15, 12) == 0xf);
							const IntRegIndex rd = (IntRegIndex)(uint32_t)bits(machInst, 19, 16);
							const IntRegIndex rn = (IntRegIndex)(uint32_t)bits(machInst, 3, 0);
							const IntRegIndex rm = (IntRegIndex)(uint32_t)bits(machInst, 11, 8);
							const IntRegIndex ra = (IntRegIndex)(uint32_t)bits(machInst, 15, 12);
							const bool m = bits(machInst, 5);
							instReg.DReg[0] = rd;
							instReg.SReg[0] = rn;
							instReg.SReg[1] = rm;
							switch (op1) {
							case 0x0:
								if (op2 == 0) {
									instType.isCc = true;
									if (aIsF) {
										if (m) {
											++instSetCounters[SmuadxCc]; return SmuadxCc;
										}
										else {
											++instSetCounters[SmuadCc]; return SmuadCc;
										}
									}
									else {
										if (m) {
											++instSetCounters[SmladxCc]; return SmladxCc;
										}
										else {
											++instSetCounters[SmladCc]; return SmladCc;
										}
									}
								}
								else if (op2 == 1) {
									if (aIsF) {
										if (m) {
											++instSetCounters[Smusdx]; return Smusdx;
										}
										else {
											++instSetCounters[Smusd]; return Smusd;
										}
									}
									else {
										instType.isCc = true;
										instReg.SReg[2] = ra;
										if (m) {
											++instSetCounters[SmlsdxCc]; return SmlsdxCc;
										}
										else {
											++instSetCounters[SmlsdCc]; return SmlsdCc;
										}
									}
								}
								break;
							case 0x1:
								instReg.SReg[2] = ra;
								if (op2 == 0 && m == 0 && ra == 0xf) {
									++instSetCounters[Sdiv]; return Sdiv;
								}
								break;
							case 0x3:
								instReg.SReg[2] = ra;
								if (op2 == 0 && m == 0 && ra == 0xf) {
									++instSetCounters[Udiv]; return Udiv;
								}
								break;
							case 0x4:
								if (op2 == 0) {
									instReg.SReg[2] = ra;
									if (m) {
										++instSetCounters[Smlal]; return Smlal;
									}
									else {
										++instSetCounters[Smlal]; return Smlal;
									}
								}
								else if (op2 == 1) {
									instReg.SReg[2] = ra;
									if (m) {
										++instSetCounters[Smlsldx]; return Smlsldx;
									}
									else {
										++instSetCounters[Smlsld]; return Smlsld;
									}
								}
								break;
							case 0x5:
								if (op2 == 0) {
									if (aIsF) {
										if (m) {
											++instSetCounters[Smmulr]; return Smmulr;
										}
										else {
											++instSetCounters[Smmul]; return Smmul;
										}
									}
									else {
										instReg.SReg[2] = ra;
										if (m) {
											++instSetCounters[Smmlar]; return Smmlar;
										}
										else {
											++instSetCounters[Smmla]; return Smmla;
										}
									}
								}
								else if (op2 == 0x3) {
									instReg.SReg[2] = ra;
									if (m) {
										++instSetCounters[Smmlsr]; return Smmlsr;
									}
									else {
										++instSetCounters[Smmls]; return Smmls;
									}
								}
								break;
							default:
								break;
							}
							++instSetCounters[Unknown32]; return Unknown32;
						}
						break;

						case 0x3:
							switch (MEDIA_OPCODE) {
							case 0x1f:
								switch (OPC2) {
								default:
									// ArmMiscMedia::armMiscMedia(([], {}))
								{
									const uint32_t op1 = bits(machInst, 22, 20);
									const uint32_t op2 = bits(machInst, 7, 5);
									const IntRegIndex rn = (IntRegIndex)(uint32_t)bits(machInst, 3, 0);
									const IntRegIndex ra = (IntRegIndex)(uint32_t)bits(machInst, 15, 12);
									instReg.DReg[0] = ra;
									instReg.SReg[0] = rn;
									if (op1 == 0 && op2 == 0) {
										const IntRegIndex rd =
											(IntRegIndex)(uint32_t)bits(machInst, 19, 16);
										const IntRegIndex rm =
											(IntRegIndex)(uint32_t)bits(machInst, 11, 8);
										instReg.SReg[1] = rm;
										instReg.DReg[0] = rd;
										if (ra == 0xf) {
											++instSetCounters[Usad8]; return Usad8;
										}
										else {
											++instSetCounters[Usada8]; return Usada8;
										}
									}
									else if (bits(op2, 1, 0) == 0x2) {
										const uint32_t lsb = bits(machInst, 11, 7);
										const uint32_t msb = lsb + bits(machInst, 20, 16);
										if (bits(op1, 2, 1) == 0x3) {
											++instSetCounters[Ubfx]; return Ubfx;
										}
										else if (bits(op1, 2, 1) == 0x1) {
											++instSetCounters[Sbfx]; return Sbfx;
										}
									}
									else if (bits(op2, 1, 0) == 0x0 && bits(op1, 2, 1) == 0x2) {
										const uint32_t lsb = bits(machInst, 11, 7);
										const uint32_t msb = bits(machInst, 20, 16);
										if (rn == 0xf) {
											++instSetCounters[Bfc]; return Bfc;
										}
										else {
											++instSetCounters[Bfi]; return Bfi;
										}
									}
									++instSetCounters[Unknown32]; return Unknown32;
								}
								break;
								}
							default:
								// ArmMiscMedia::armMiscMedia(([], {}))
							{
								const uint32_t op1 = bits(machInst, 22, 20);
								const uint32_t op2 = bits(machInst, 7, 5);
								const IntRegIndex rn = (IntRegIndex)(uint32_t)bits(machInst, 3, 0);
								const IntRegIndex ra = (IntRegIndex)(uint32_t)bits(machInst, 15, 12);
								instReg.SReg[0] = rn;
								instReg.DReg[0] = ra;
								if (op1 == 0 && op2 == 0) {
									const IntRegIndex rd = (IntRegIndex)(uint32_t)bits(machInst, 19, 16);
									const IntRegIndex rm = (IntRegIndex)(uint32_t)bits(machInst, 11, 8);
									instReg.SReg[1] = rm;
									instReg.DReg[0] = rd;
									if (ra == 0xf) {
										++instSetCounters[Usad8]; return Usad8;
									}
									else {
										++instSetCounters[Usada8]; return Usada8;
									}
								}
								else if (bits(op2, 1, 0) == 0x2) {
									const uint32_t lsb = bits(machInst, 11, 7);
									const uint32_t msb = lsb + bits(machInst, 20, 16);
									if (bits(op1, 2, 1) == 0x3) {
										++instSetCounters[Ubfx]; return Ubfx;
									}
									else if (bits(op1, 2, 1) == 0x1) {
										++instSetCounters[Sbfx]; return Sbfx;
									}
								}
								else if (bits(op2, 1, 0) == 0x0 && bits(op1, 2, 1) == 0x2) {
									const uint32_t lsb = bits(machInst, 11, 7);
									const uint32_t msb = bits(machInst, 20, 16);
									if (rn == 0xf) {
										++instSetCounters[Bfc]; return Bfc;
									}
									else {
										++instSetCounters[Bfi]; return Bfi;
									}
								}
								++instSetCounters[Unknown32]; return Unknown32;
							}
							break;
							}
						default:
							// Unknown32::unknown(([], {}))
							++instSetCounters[Unknown32]; return Unknown32;
							break;
						}

					default:

						// Unknown32::unknown(([], {}))
						++instSetCounters[Unknown32]; return Unknown32;
						break;
					}

				case 0x4:
					return ArmMacroMem(machInst, instSetCounters, instReg, instType);

					//++instSetCounters[LdmStm]; return LdmStm; // exception return
					break;

				case 0x5:
					switch (OPCODE_24) {

					case 0x0:
						// ArmBBlxImm::armBBlxImm(([], {}))
						instType.isBranch = true;
						if (COND_CODE == 0xF) {
							int32_t imm = (sext<26>(bits(machInst, 23, 0) << 2)) |
								(bits(machInst, 24) << 1);
							++instSetCounters[BlxImm]; return BlxImm;
						}
						else {
							++instSetCounters[B]; return B;
						}
						break;

					case 0x1:
						// ArmBlBlxImm::armBlBlxImm(([], {}))
						instType.isBranch = true;
						if (COND_CODE == 0xF) {
							int32_t imm = (sext<26>(bits(machInst, 23, 0) << 2)) |
								(bits(machInst, 24) << 1);
							++instSetCounters[BlxImm]; return BlxImm;
						}
						else {
							++instSetCounters[B]; return B;
						}
						break;
					default:
						// Unknown32::unknown(([], {}))
						++instSetCounters[Unknown32]; return Unknown32;
						break;
					}

				case 0x6:
					switch (CPNUM) {
					case 0xa: case 0xb:
						// ExtensionRegLoadStore::extensionRegLoadStore(([], {}))
						return decodeExtensionRegLoadStore(machInst, instSetCounters, instReg, instType);
						break;
					case 0xf:
						switch (OPCODE_20) {
						case 0x0:
							// Mcrr15::Mcrr15(([], {}))
							return decodeMcrrMrrc15(machInst, instSetCounters, instReg, instType);
							break;

						case 0x1:
							// Mrrc15::Mrrc15(([], {}))
							return decodeMcrrMrrc15(machInst, instSetCounters, instReg, instType);
							break;

						default:
							// Unknown32::unknown(([], {}))
							++instSetCounters[Unknown32]; return Unknown32;
							break;
						}
					default:
						// Unknown32::unknown(([], {}))
						++instSetCounters[Unknown32]; return Unknown32;
						break;
					}

				case 0x7:
					switch (OPCODE_24) {
					case 0x0:
						switch (OPCODE_4) {
						case 0x0:
							switch (CPNUM) {
							case 0xa: case 0xb:
								// VfpData::vfpData(([], {}))

								return decodeVfpData(machInst, instSetCounters, instReg, instType);
								break;

							default:

								// Unknown32::unknown(([], {}))
								++instSetCounters[Unknown32]; return Unknown32;
								break;
							}

						case 0x1:
							switch (CPNUM) {

							case 0x1:
								return M5ops(machInst, instSetCounters, instReg, instType);
								break;

							case 0xa: case 0xb:
								// ShortFpTransfer::shortFpTransfer(([], {}))

								return decodeShortFpTransfer(machInst, instSetCounters, instReg, instType);
								break;

							case 0xe:
								// McrMrc14::mcrMrc14(([], {}))

								return decodeMcrMrc14(machInst, instSetCounters, instReg, instType);
								break;

							case 0xf:
								// McrMrc15::mcrMrc15(([], {}))

								return decodeMcrMrc15(machInst, instSetCounters, instReg, instType);
								break;

							default:

								// Unknown32::unknown(([], {}))
								++instSetCounters[Unknown32]; return Unknown32;
								break;
							}

						default:

							// Unknown32::unknown(([], {}))
							++instSetCounters[Unknown32]; return Unknown32;
							break;
						}

					case 0x1:
						// Svc::svc(([], {}))
						++instSetCounters[Svc]; return Svc;

					default:

						// Unknown32::unknown(([], {}))
						++instSetCounters[Unknown32]; return Unknown32;
						break;
					}

				default:
					// Unknown32::unknown(([], {}))
					++instSetCounters[Unknown32]; return Unknown32;
					break;
		}
	}

	++instSetCounters[Unknown32]; return Unknown32;
}

Aarch32Set aarch32::decodeNeonMem(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType)
{
	const uint32_t b = bits(machInst, 11, 8);
	const bool single = bits(machInst, 23);
	const bool singleAll = single && (bits(b, 3, 2) == 3);
	const bool load = bits(machInst, 21);
	unsigned width = 0;		//[pz]width is equal to element number
	if (single) {
		width = bits(b, 1, 0) + 1;
	}
	else {
		switch (bits(b, 3, 1)) {
		case 0x0: width = 4;
			break;
		case 0x1: width = (b & 0x1) ? 2 : 1;
			break;
		case 0x2: width = 3;
			break;
		case 0x3: width = 1;
			break;
		case 0x4: width = 2;
			break;
		case 0x5:
			if ((b & 0x1) == 0) {
				width = 1;
				break;
			}
			// Fall through on purpose.
		default:
			++instSetCounters[Unknown32]; return Unknown32;;
		}
	}

	const IntRegIndex rm = (IntRegIndex)(uint32_t)bits(machInst, 3, 0);
	const IntRegIndex rn = (IntRegIndex)(uint32_t)bits(machInst, 19, 16);
	uint32_t vd = (uint32_t)(bits(machInst, 15, 12) |
		bits(machInst, 22) << 4);
	const uint32_t type = bits(machInst, 11, 8);
	uint32_t size = 0;
	uint32_t align = 0;
	unsigned inc = 1;
	unsigned regs = 1;
	unsigned lane = 0;
	if (single) {
		if (singleAll) {
			size = bits(machInst, 7, 6);
			bool t = bits(machInst, 5);
			align = size ;
			if (width == 1) {
				regs = t ? 2 : 1;
				inc = 1;
			}
			else {
				regs = width;
				inc = t ? 2 : 1;
			}
			switch (width) {
			case 1:
			case 2:
				if (bits(machInst, 4))
					align = size + width - 1;
				break;
			case 3:
				break;
			case 4:
				if (size == 3) {
					if (bits(machInst, 4) == 0)
						++instSetCounters[Unknown32]; return Unknown32;;
					size = 2;
					align = 0x4;
				}
				else if (size == 2) {
					if (bits(machInst, 4))
						align = 0x3;
				}
				else {
					if (bits(machInst, 4))
						align = size + 2;
				}
				break;
			}
		}
		else {
			size = bits(machInst, 11, 10);
			align = size ;
			regs = width;
			unsigned indexAlign = bits(machInst, 7, 4);
			// If width is 1, inc is always 1. That's overridden later.
			switch (size) {
			case 0:
				inc = 1;
				lane = bits(indexAlign, 3, 1);
				break;
			case 1:
				inc = bits(indexAlign, 1) ? 2 : 1;
				lane = bits(indexAlign, 3, 2);
				break;
			case 2:
				inc = bits(indexAlign, 2) ? 2 : 1;
				lane = bits(indexAlign, 3);
				break;
			}
			// Override inc for width of 1.
			if (width == 1) {
				inc = 1;
			}
			switch (width) {
			case 1:
				switch (size) {
				case 0:
					break;
				case 1:
					if (bits(indexAlign, 0))
						align = 1;
					break;
				case 2:
					if (bits(indexAlign, 1, 0))
						align = 2;
					break;
				}
				break;
			case 2:
				if (bits(indexAlign, 0))
					align = size + 1;
				break;
			case 3:
				break;
			case 4:
				switch (size) {
				case 0:
				case 1:
					if (bits(indexAlign, 0))
						align = size + 2;
					break;
				case 2:
					if (bits(indexAlign, 0))
						align = bits(indexAlign, 1, 0) + 2;
					break;
				}
				break;
			}
		}
		if (size == 0x3) {
			++instSetCounters[Unknown32]; return Unknown32;;
		}
	}
	else {
		size = bits(machInst, 7, 6);
		align = bits(machInst, 5, 4);
		if (align == 0) {
			// @align wasn't specified, so alignment can be turned off.
			align = size ;
		}
		else {
			align = align + 2;
		}
		switch (width) {
		case 1:
			switch (type) {
			case 0x7: regs = 1;
				break;
			case 0xa: regs = 2;
				break;
			case 0x6: regs = 3;
				break;
			case 0x2: regs = 4;
				break;
			default:
				++instSetCounters[Unknown32]; return Unknown32;;
			}
			break;
		case 2:
			// Regs doesn't behave exactly as it does in the manual
			// because they loop over regs registers twice and we break
			// it down in the macroop.
			switch (type) {
			case 0x8: regs = 2; inc = 1;
				break;
			case 0x9: regs = 2; inc = 2;
				break;
			case 0x3: regs = 4; inc = 2;
				break;
			default:
				++instSetCounters[Unknown32]; return Unknown32;;
			}
			break;
		case 3:
			regs = 3;
			switch (type) {
			case 0x4: inc = 1;
				break;
			case 0x5: inc = 2;;
				break;
			default:
				++instSetCounters[Unknown32]; return Unknown32;;
			}
			break;
		case 4:
			regs = 4;
			switch (type) {
			case 0: inc = 1;
				break;
			case 1: inc = 2;
				break;
			default:
				++instSetCounters[Unknown32]; return Unknown32;;
			}
			break;
		}
	}
	//[modified by pz]寄存器数量可能有问题
	if (load) {
		// Load instructions.
		instType.isLoad = true;
		for (int i = 0; i < regs; i++) {
			instReg.DReg[2 + i] = (IntRegIndex)vd;
			vd += inc;
		}
		if (single) {						
			instReg.SReg[0] = rn;
			instReg.SReg[1] = rm;
			++instSetCounters[VldSingle]; return VldSingle;
		}
		else {
			instReg.SReg[0] = rn;
			instReg.SReg[1] = rm;
			++instSetCounters[VldMult]; return VldMult;
		}
	}
	else {
		// Store instructions.
		instType.isStore = true;

		if (single) {
			if (singleAll) {
				++instSetCounters[Unknown32]; return Unknown32;;
			}
			else {
				for (int i = 0; i < regs; i++) {
					instReg.SReg[3 + i] = (IntRegIndex)vd;
					vd += inc;
				}
				instReg.SReg[0] = rn;
				instReg.SReg[1] = rm;
				++instSetCounters[VstSingle]; return VstSingle;
			}
		}
		else {
			for (int i = 0; i < regs; i++) {
				instReg.SReg[3 + i] = (IntRegIndex)vd;
				vd += inc;
			}
			instReg.SReg[0] = rn;
			instReg.SReg[1] = rm;
			++instSetCounters[VstMult]; return VstMult;
		}
	}
	++instSetCounters[Unknown32]; return Unknown32;;
}

inline Aarch32Set aarch32::Thumb16ShiftAddSubMoveCmp(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const uint8_t imm5 = bits(machInst, 10, 6);
	const uint8_t imm3 = bits(machInst, 8, 6);
	const uint8_t imm8 = bits(machInst, 7, 0);
	const IntRegIndex rd = (IntRegIndex)(uint8_t)bits(machInst, 2, 0);
	const IntRegIndex rd8 = (IntRegIndex)(uint8_t)bits(machInst, 10, 8);
	const IntRegIndex rn = (IntRegIndex)(uint8_t)bits(machInst, 5, 3);
	const IntRegIndex rm = (IntRegIndex)(uint8_t)bits(machInst, 8, 6);

	//added by WenjFu
	//uint8_t regDSPSR = read(MISCREG_DSPSR_EL0);
	//uint8_t itstate = (bits(regDSPSR,15,10) << 6) | (bits(regDSPSR,26,25));
	//if(bits(itstate,3,0) == 0)
	//    bool itstateMask = false;
	//else
	//    bool itstateMask = true;
	// can not complete!
	bool itstateMask = false;
	//added end

	switch (bits(machInst, 13, 11)) {
	case 0x0: // lsl
	{
		instReg.DReg[0] = rd;
		instReg.SReg[1] = rn;
		if (itstateMask) {
			{++instSetCounters[MovReg]; return MovReg; }
		}
		else {
			{++instSetCounters[MovRegCc]; return MovRegCc; }
		}
	}
	case 0x1: // lsr
	{
		instReg.DReg[0] = rd;
		instReg.SReg[1] = rn;
		if (itstateMask)
		{
			++instSetCounters[MovReg]; return MovReg;
		}
		else
		{
			++instSetCounters[MovRegCc]; return MovRegCc;
		}
	}
	case 0x2: // asr
	{
		instReg.DReg[0] = rd;
		instReg.SReg[1] = rn;
		if (itstateMask)
		{
			++instSetCounters[MovReg]; return MovReg;
		}
		else
		{
			++instSetCounters[MovRegCc]; return MovRegCc;
		}
	}
	case 0x3:
		switch (bits(machInst, 10, 9)) {
		case 0x0:
		{
			instReg.DReg[0] = rd;
			instReg.SReg[1] = rn;
			instReg.SReg[2] = rm;
			if (itstateMask) {
				{++instSetCounters[AddReg]; return AddReg; }
			}
			else {
				{++instSetCounters[AddRegCc]; return AddRegCc; }
			}
		}
		case 0x1:
		{
			instReg.DReg[0] = rd;
			instReg.SReg[1] = rn;
			instReg.SReg[2] = rm;
			if (itstateMask) {
				{++instSetCounters[SubReg]; return SubReg; }
			}
			else {
				{++instSetCounters[SubRegCc]; return SubRegCc; }
			}
		}
		case 0x2:
		{
			instReg.DReg[0] = rd;
			instReg.SReg[1] = rn;
			if (itstateMask) {
				{++instSetCounters[AddImm]; return AddImm; }
			}
			else {
				{++instSetCounters[AddImmCc]; return AddImmCc; }
			}
		}
		case 0x3:
		{
			instReg.DReg[0] = rd;
			instReg.SReg[1] = rn;
			if (itstateMask)
			{
				++instSetCounters[SubImm]; return SubImm;
			}
			else
			{
				++instSetCounters[SubImmCc]; return SubImmCc;
			}
		}
		}
	case 0x4:
	{
		instReg.DReg[0] = rd8;
		if (itstateMask)
		{
			++instSetCounters[MovImm]; return MovImm;
		}
		else
		{
			++instSetCounters[MovImmCc]; return MovImmCc;
		}
	}
	case 0x5:
	{
		instReg.DReg[0] = rd8;
		{++instSetCounters[CmpImmCc]; return CmpImmCc; }
	}
	case 0x6:
	{
		instReg.DReg[0] = rd8;
		instReg.SReg[1] = rd8;
		if (itstateMask)
		{
			++instSetCounters[AddImm]; return AddImm;
		}
		else
		{
			++instSetCounters[AddImmCc]; return AddImmCc;
		}
	}
	case 0x7:
	{
		instReg.DReg[0] = rd8;
		instReg.SReg[1] = rd8;
		if (itstateMask)
		{
			++instSetCounters[SubImm]; return SubImm;
		}
		else
		{
			++instSetCounters[SubImmCc]; return SubImmCc;
		}
	}
	}
	{++instSetCounters[Unknown32]; return Unknown32; }
}

inline Aarch32Set aarch32::Thumb16DataProcessing(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const IntRegIndex rdn = (IntRegIndex)(uint8_t)bits(machInst, 2, 0);
	const IntRegIndex rm = (IntRegIndex)(uint8_t)bits(machInst, 5, 3);

	//added by WenjFu
	//uint8_t regDSPSR = read(MISCREG_DSPSR_EL0);
	//uint8_t itstate = (bits(regDSPSR,15,10) << 6) | (bits(regDSPSR,26,25));
	//if(bits(itstate,3,0) == 0)
	//    bool itstateMask = false;
	//else
	//    bool itstateMask = true;
	// can not complete!
	bool itstateMask = false;
	//added end

	instReg.DReg[0] = rdn;
	instReg.SReg[1] = rm;
	switch (bits(machInst, 9, 6)) {
	case 0x0:
	{
		instReg.SReg[0] = rdn;
		if (itstateMask)
		{
			++instSetCounters[AndReg]; return AndReg;
		}
		else
		{
			++instSetCounters[AndRegCc]; return AndRegCc;
		}
	}
	case 0x1:
	{
		instReg.SReg[1] = rdn;
		if (itstateMask)
		{
			++instSetCounters[EorReg]; return EorReg;
		}
		else
		{
			++instSetCounters[EorRegCc]; return EorRegCc;
		}
	}
	case 0x2: //lsl
	{
		instReg.SReg[1] = rdn;
		if (itstateMask)
		{
			++instSetCounters[MovRegReg]; return MovRegReg;
		}
		else
		{
			++instSetCounters[MovRegRegCc]; return MovRegRegCc;
		}
	}
	case 0x3: //lsr
	{
		instReg.SReg[1] = rdn;
		if (itstateMask)
		{
			++instSetCounters[MovRegReg]; return MovRegReg;
		}
		else
		{
			++instSetCounters[MovRegRegCc]; return MovRegRegCc;
		}
	}
	case 0x4: //asr
	{
		instReg.SReg[1] = rdn;
		if (itstateMask)
		{
			++instSetCounters[MovRegReg]; return MovRegReg;
		}
		else
		{
			++instSetCounters[MovRegRegCc]; return MovRegRegCc;
		}
	}
	case 0x5:
	{
		instReg.SReg[1] = rdn;
		if (itstateMask)
		{
			++instSetCounters[AdcReg]; return AdcReg;
		}
		else
		{
			++instSetCounters[AdcRegCc]; return AdcRegCc;
		}
	}
	case 0x6:
	{
		instReg.SReg[1] = rdn;
		if (itstateMask)
		{
			++instSetCounters[SbcReg]; return SbcReg;
		}
		else
		{
			++instSetCounters[SbcRegCc]; return SbcRegCc;
		}
	}
	case 0x7: // ror
	{
		instReg.SReg[1] = rdn;
		if (itstateMask)
		{
			++instSetCounters[MovRegReg]; return MovRegReg;
		}
		else
		{
			++instSetCounters[MovRegRegCc]; return MovRegRegCc;
		}
	}
	case 0x8:{
		{++instSetCounters[TstRegCc]; return TstRegCc; }
	}
	case 0x9:
	{
		if (itstateMask)
		{
			++instSetCounters[RsbImm]; return RsbImm;
		}
		else
		{
			++instSetCounters[RsbImmCc]; return RsbImmCc;
		}
	}
	case 0xa:{
		{++instSetCounters[CmpRegCc]; return CmpRegCc; }
	}
	case 0xb:{
		instReg.DReg[0] = rdn;
		instReg.SReg[0] = rm;
		{++instSetCounters[CmnRegCc]; return CmnRegCc; }
	}
	case 0xc:
	{
		instReg.SReg[1] = rdn;
		if (itstateMask)
		{
			++instSetCounters[OrrReg]; return OrrReg;
		}
		else
		{
			++instSetCounters[OrrRegCc]; return OrrRegCc;
		}
	}
	case 0xd:
	{
		instReg.SReg[1] = rdn;
		if (itstateMask)
		{
			++instSetCounters[Mul]; return Mul;
		}
		else
		{
			++instSetCounters[MulCc]; return MulCc;
		}
	}
	case 0xe:
	{
		instReg.SReg[1] = rdn;
		if (itstateMask)
		{
			++instSetCounters[BicReg]; return BicReg;
		}
		else
		{
			++instSetCounters[BicRegCc]; return BicRegCc;
		}
	}
	case 0xf:
	{
		if (itstateMask)
		{
			++instSetCounters[MvnReg]; return MvnReg;
		}
		else
		{
			++instSetCounters[MvnRegCc]; return MvnRegCc;
		}
	}
	}
	{++instSetCounters[Unknown32]; return Unknown32; }
}

inline Aarch32Set aarch32::Thumb16SpecDataAndBx(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const IntRegIndex rdn = (IntRegIndex)(uint8_t)(bits(machInst, 2, 0) | (bits(machInst, 7) << 3));
	const IntRegIndex rm = (IntRegIndex)(uint8_t)bits(machInst, 6, 3);
	switch (bits(machInst, 9, 8)) {
	case 0x0:{
		instReg.DReg[0] = rdn;
		instReg.SReg[0] = rdn;
		instReg.SReg[1] = rm;
		{++instSetCounters[AddReg]; return AddReg; }
	}
	case 0x1:{
		instReg.DReg[0] = rdn;
		instReg.SReg[0] = rm;
		{++instSetCounters[CmpRegCc]; return CmpRegCc; }
	}
	case 0x2:{
		instReg.DReg[0] = rdn;
		instReg.SReg[0] = rm;
		{++instSetCounters[MovReg]; return MovReg; }
	}
	case 0x3:{
		instReg.SReg[0] = rm;
		if (bits(machInst, 7) == 0)
		{
			++instSetCounters[BxReg]; return BxReg;
		}
		else
		{
			++instSetCounters[BlxReg]; return BlxReg;
		}
	}
	}
	{++instSetCounters[Unknown32]; return Unknown32; }
}

inline Aarch32Set aarch32::Thumb16MemLit(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const uint8_t rt = bits(machInst, 10, 8);
	const uint8_t imm8 = bits(machInst, 7, 0);
	instReg.DReg[0] = rt;
	{instType.isLoad = true; ++instSetCounters[Ldrlit]; return Ldrlit; }
}

inline Aarch32Set aarch32::Thumb16MemReg(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const uint8_t opb = bits(machInst, 11, 9);
	const uint8_t rt = bits(machInst, 2, 0);
	const uint8_t rn = bits(machInst, 5, 3);
	const uint8_t rm = bits(machInst, 8, 6);
	instReg.DReg[0] = rn;
	instReg.SReg[0] = rm;
	instReg.SReg[1] = rt;
	instReg.SReg[2] = rn;
	switch (opb) {
	case 0x0:
	{instType.isStore = true; ++instSetCounters[Str]; return Str; }
	case 0x1:
	{instType.isStore = true; ++instSetCounters[Strh]; return Strh; }
	case 0x2:
	{instType.isStore = true; ++instSetCounters[Strb]; return Strb; }
	case 0x3:
	{instType.isLoad = true; ++instSetCounters[Ldrsb]; return Ldrsb; }
	case 0x4:
	{instType.isLoad = true; ++instSetCounters[Ldr]; return Ldr; }
	case 0x5:
	{instType.isLoad = true; ++instSetCounters[Ldrh]; return Ldrh; }
	case 0x6:
	{instType.isLoad = true; ++instSetCounters[Ldrb]; return Ldrb; }
	case 0x7:
	{instType.isLoad = true; ++instSetCounters[Ldrsh]; return Ldrsh; }
	}
	{++instSetCounters[Unknown32]; return Unknown32; }
}

inline Aarch32Set aarch32::Thumb16MemImm(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const uint8_t opa = bits(machInst, 15, 12);
	const uint8_t opb = bits(machInst, 11, 9);
	const uint8_t lrt = bits(machInst, 2, 0);
	const uint8_t lrn = bits(machInst, 5, 3);
	const uint8_t hrt = bits(machInst, 10, 8);
	const uint8_t imm5 = bits(machInst, 10, 6);
	const uint8_t imm8 = bits(machInst, 7, 0);
	const bool load = bits(opb, 2) ? true : false;
	switch (opa) {
	case 0x6:{
		if (load){
			instReg.DReg[0] = lrt;
			instReg.SReg[0] = lrn;
			instType.isLoad = true;
			{++instSetCounters[Ldr]; return Ldr; }
		}
		else{
			instReg.SReg[0] = lrt;
			instReg.SReg[1] = lrn;
			instType.isStore = true;
			{++instSetCounters[Str]; return Str; }
		}
	}
	case 0x7:{
		if (load){
			instReg.DReg[0] = lrt;
			instReg.SReg[0] = lrn;
			instType.isLoad = true;
			{++instSetCounters[Ldrb]; return Ldrb; }
		}
		else{
			instReg.SReg[0] = lrt;
			instReg.SReg[1] = lrn;
			instType.isStore = true;
			{++instSetCounters[Strb]; return Strb; }
		}
	}
	case 0x8:{
		if (load){
			instReg.DReg[0] = lrt;
			instReg.SReg[0] = lrn;
			instType.isLoad = true;
			{++instSetCounters[Ldrh]; return Ldrh; }
		}
		else{
			instReg.SReg[0] = lrt;
			instReg.SReg[1] = lrn;
			instType.isStore = true;
			{++instSetCounters[Strh]; return Strh; }
		}
	}
	case 0x9:{
		if (load){
			instReg.DReg[0] = hrt;
			instReg.SReg[0] = 13;
			instType.isLoad = true;
			{++instSetCounters[LdrS]; return LdrS; }
		}
		else{
			instReg.SReg[0] = hrt;
			instReg.SReg[1] = 13;
			instType.isStore = true;
			{++instSetCounters[StrS]; return StrS; }
		}
	}
	default:
	{++instSetCounters[Unknown32]; return Unknown32; }
	}
}

inline Aarch32Set aarch32::Thumb16Adr(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const IntRegIndex rd = (IntRegIndex)(uint8_t)bits(machInst, 10, 8);
	const uint8_t imm8 = bits(machInst, 7, 0) << 2;
	instReg.DReg[0] = rd;
	{++instSetCounters[AdrImm]; return AdrImm; }
}

inline Aarch32Set aarch32::Thumb16AddSp(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const IntRegIndex rd = (IntRegIndex)(uint8_t)bits(machInst, 10, 8);
	const uint8_t imm8 = bits(machInst, 7, 0) << 2;
	instReg.DReg[0] = rd;
	{++instSetCounters[AddImmS]; return AddImmS; }
}

inline Aarch32Set aarch32::Thumb16Misc(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	instType.isMISC = true;
	switch (bits(machInst, 11, 8)) {
	case 0x0:{
		instReg.DReg[0] = 13;
		if (bits(machInst, 7))
		{
			++instSetCounters[SubImmS]; return SubImmS;
		}
		else
		{
			++instSetCounters[AddImmS]; return AddImmS;
		}
	}
	case 0x2:{
		const IntRegIndex rd = (IntRegIndex)(uint8_t)bits(machInst, 2, 0);
		const IntRegIndex rm = (IntRegIndex)(uint8_t)bits(machInst, 5, 3);
		instReg.DReg[0] = rd;
		instReg.SReg[0] = rm;
		switch (bits(machInst, 7, 6)) {
		case 0x0:
		{++instSetCounters[Sxth]; return Sxth; }
		case 0x1:
		{++instSetCounters[Sxtb]; return Sxtb; }
		case 0x2:
		{++instSetCounters[Uxth]; return Uxth; }
		case 0x3:
		{++instSetCounters[Uxtb]; return Uxtb; }
		}
	}
	case 0x1:
	case 0x3:{
		const IntRegIndex rn0 = (IntRegIndex)(uint8_t)bits(machInst, 2, 0);
		const uint8_t imm6 = (bits(machInst, 9) << 6) | (bits(machInst, 7, 3) << 1);
		instReg.SReg[0] = rn0;
		{++instSetCounters[Cbz]; return Cbz; }
	}
	case 0x4:
	case 0x5:
	{
		const uint8_t m = bits(machInst, 8);
		const uint8_t regList = bits(machInst, 7, 0) | (m << 14);
		const IntRegIndex rn = (IntRegIndex)(uint8_t)bits(machInst, 10, 8);
		const bool load = bits(machInst, 11) ? true : false;
		instReg.SReg[0] = rn;
		if (load){
			instType.isLoad = true;
			{++instSetCounters[LdmStmLd]; return LdmStmLd; }
		}
		else{
			instType.isStore = true;
			{++instSetCounters[LdmStmSt]; return LdmStmSt; }
		}
	}
	case 0x6:
	{
		const uint8_t opBits = bits(machInst, 7, 5);
		if (opBits == 2)
		{
			++instSetCounters[Setend]; return Setend;
		}
		else if (opBits == 3) {
			const bool enable = (bits(machInst, 4) == 0);
			const uint8_t mods = (bits(machInst, 2, 0) << 5) | ((enable ? 1 : 0) << 9);
			{++instSetCounters[Cps]; return Cps; }
		}
	}
	case 0xa:
	{
		IntRegIndex rd = (IntRegIndex)(uint8_t)bits(machInst, 2, 0);
		IntRegIndex rm = (IntRegIndex)(uint8_t)bits(machInst, 5, 3);
		instReg.DReg[0] = rd;
		instReg.SReg[0] = rm;
		switch (bits(machInst, 7, 6)) {
		case 0x0:
		{++instSetCounters[Rev]; return Rev; }
		case 0x1:
		{++instSetCounters[Rev16]; return Rev16; }
		case 0x3:
		{++instSetCounters[Revsh]; return Revsh; }
		default:
			break;
		}
	}
	break;
	case 0x9:
	case 0xb:{
		const IntRegIndex rn0 = (IntRegIndex)(uint8_t)bits(machInst, 2, 0);
		const uint8_t imm6 = (bits(machInst, 9) << 6) | (bits(machInst, 7, 3) << 1);
		instReg.SReg[0] = rn0;
		{++instSetCounters[Cbnz]; return Cbnz; }
	}
	case 0xc:
	case 0xd:
	{
		const uint8_t p = bits(machInst, 8);
		const uint8_t regList = bits(machInst, 7, 0) | (p << 15);
		const IntRegIndex rn = (IntRegIndex)(uint8_t)bits(machInst, 10, 8);
		const bool load = bits(machInst, 11) ? true : false;
		instReg.SReg[0] = rn;
		if (load){
			instType.isLoad = true;
			{++instSetCounters[LdmStmLd]; return LdmStmLd; }
		}
		else{
			instType.isStore = true;
			{++instSetCounters[LdmStmSt]; return LdmStmSt; }
		}
	}
	case 0xe:
	{++instSetCounters[BkptInst]; return BkptInst; }
	case 0xf:
		if (bits(machInst, 3, 0) != 0)
		{
			++instSetCounters[ItInst]; return ItInst;
		}
		switch (bits(machInst, 7, 4)) {
		case 0x0:
		{++instSetCounters[NopInst]; return NopInst; }
		case 0x1:
		{++instSetCounters[YieldInst]; return YieldInst; }
		case 0x2:
		{++instSetCounters[WfeInst]; return WfeInst; }
		case 0x3:
		{++instSetCounters[WfiInst]; return WfiInst; }
		case 0x4:
		{++instSetCounters[SevInst]; return SevInst; }
		default:
		{++instSetCounters[FailUnimplemented]; return FailUnimplemented; }// new WarnUnimplemented("unallocated_hint", machInst);
		}
	default:
		break;
	}
	{++instSetCounters[Unknown32]; return Unknown32; }
}

inline Aarch32Set aarch32::Thumb16MacroMem(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const IntRegIndex rn = (IntRegIndex)(uint8_t)bits(machInst, 10, 8);
	const bool load = (bits(machInst, 11) == 1);
	const uint8_t regList = bits(machInst, 7, 0);
	const bool writeback = (!load || bits(regList, rn) == 0);
	instReg.SReg[0] = rn;
	if (writeback) instReg.DReg[0] = rn;
	if (load){
		instType.isLoad = true;
		{++instSetCounters[LdmStmLd]; return LdmStmLd; }
	}
	else{
		instType.isStore = true;
		{++instSetCounters[LdmStmSt]; return LdmStmSt; }
	}
}

inline Aarch32Set aarch32::Thumb16CondBranchAndSvc(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	instType.isBranch = true;
	if (bits(machInst, 11, 9) != 0x7)
	{
		instType.isCc = true; ++instSetCounters[B]; return B;
	}
	else if (bits(machInst, 8))
	{
		++instSetCounters[Svc]; return Svc;
	}
	else
	{
		++instSetCounters[Unknown32]; return Unknown32;
	}// This space will not be allocated in the future.
}

inline Aarch32Set aarch32::Thumb16UncondBranch(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	instType.isBranch = true;
	{++instSetCounters[B]; return B; }
}

inline Aarch32Set aarch32::Thumb32SrsRfe(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const bool wb = (bits(machInst, 21) == 1);
	const bool add = (bits(machInst, 24, 23) == 0x3);
	if (bits(machInst, 20) == 1) {
		// post == add
		const IntRegIndex rn = (IntRegIndex)(uint8_t)bits(machInst, 19, 16);
		instReg.SReg[0] = rn;
		instType.isLoad = true;
		if (!add && !wb)
		{
			++instSetCounters[Rfe]; return Rfe;
		}
		else if (add && !wb)
		{
			++instSetCounters[Rfe_u]; return Rfe_u;
		}
		else if (!add && wb){
			instReg.DReg[0] = rn;
			{++instSetCounters[Rfe_w]; return Rfe_w; }
		}
		else{
			instReg.DReg[0] = rn;
			{++instSetCounters[Rfe_uw]; return Rfe_uw; }
		}
	}
	else {
		const uint8_t mode = bits(machInst, 4, 0);
		instType.isStore = true;
		if (badMode32((OperatingMode)mode))
		{
			++instSetCounters[Unknown32]; return Unknown32;
		}
		if (!add && !wb)
		{
			++instSetCounters[Srs]; return Srs;
		}
		else if (add && !wb)
		{
			++instSetCounters[Srs_u]; return Srs_u;
		}
		else if (!add && wb)
		{
			++instSetCounters[Srs_w]; return Srs_w;
		}
		else
		{
			++instSetCounters[Srs_uw]; return Srs_uw;
		}
	}
}

inline Aarch32Set aarch32::Thumb32LdrStrDExTbh(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const uint8_t op1 = bits(machInst, 24, 23);
	const uint8_t op2 = bits(machInst, 21, 20);
	const uint8_t op3 = bits(machInst, 7, 4);
	const IntRegIndex rn = (IntRegIndex)(uint8_t)bits(machInst, 19, 16);
	const IntRegIndex rt = (IntRegIndex)(uint8_t)bits(machInst, 15, 12);
	const IntRegIndex rt2 = (IntRegIndex)(uint8_t)bits(machInst, 11, 8);
	const IntRegIndex rd = (IntRegIndex)(uint8_t)bits(machInst, 3, 0);
	const uint8_t imm8 = bits(machInst, 7, 0);
	if (bits(op1, 1) == 0 && bits(op2, 1) == 0) {
		if (op1 == 0) {
			const uint8_t imm = bits(machInst, 7, 0) << 2;
			if (op2 == 0) {
				instReg.DReg[0] = rt2;
				instReg.SReg[0] = rt;
				instReg.SReg[1] = rn;

				{instType.isStore = true; ++instSetCounters[Strex]; return Strex; }
			}
			else {
				instReg.DReg[0] = rt;
				instReg.SReg[0] = rn;
				{instType.isLoad = true; ++instSetCounters[Ldrex]; return Ldrex; }
			}
		}
		else {
			if (op2 == 0) {
				switch (op3) {
					instReg.DReg[0] = rd;
					instReg.SReg[0] = rt;
					instReg.SReg[1] = rn;
				case 0x4:
				{instType.isStore = true; ++instSetCounters[Strexb]; return Strexb; }
				case 0x5:
				{instType.isStore = true; ++instSetCounters[Strexh]; return Strexh; }
				case 0x7:{
					instReg.SReg[2] = rt2;
					{instType.isStore = true; ++instSetCounters[Strexd]; return Strexd; }
				}
				default:
				{++instSetCounters[Unknown32]; return Unknown32; }
				}
			}
			else {
				switch (op3) {
				case 0x0:{
					instReg.SReg[0] = rd;
					instReg.SReg[1] = rn;
					{++instSetCounters[Tbb]; return Tbb; }
				}
				case 0x1:{
					instReg.SReg[0] = rd;
					instReg.SReg[1] = rn;
					{++instSetCounters[Tbh]; return Tbh; }
				}
				case 0x4:{
					instReg.DReg[0] = rt;
					instReg.SReg[0] = rn;
					{instType.isLoad = true; ++instSetCounters[Ldrexb]; return Ldrexb; }
				}
				case 0x5:{
					instReg.DReg[0] = rt;
					instReg.SReg[0] = rn;
					{instType.isLoad = true; ++instSetCounters[Ldrexh]; return Ldrexh; }
				}
				case 0x7:{
					instReg.DReg[0] = rt;
					instReg.DReg[1] = rt2;
					instReg.SReg[0] = rn;
					{instType.isLoad = true; ++instSetCounters[Ldrexd]; return Ldrexd; }
				}
				default:
				{++instSetCounters[Unknown32]; return Unknown32; }
				}
			}
		}
	}
	else {
		const uint8_t puw = (bits(machInst, 24, 23) << 1) | bits(machInst, 21);
		const uint8_t dimm = imm8 << 2;
		if (bits(op2, 0) == 0) {
			instReg.SReg[0] = rn;
			instReg.SReg[1] = rt;
			instReg.SReg[2] = rt2;
			if (bits(puw, 0)) instReg.DReg[0] = rn;
			instType.isStore = true;
			switch (puw) {
			case 0x1:
			{++instSetCounters[Strd_w]; return Strd_w; }
			case 0x3:
			{++instSetCounters[Strd_uw]; return Strd_uw; }
			case 0x4:
			{++instSetCounters[Strd_p]; return Strd_p; }
			case 0x5:
			{++instSetCounters[Strd_pw]; return Strd_pw; }
			case 0x6:
			{++instSetCounters[Strd_pu]; return Strd_pu; }
			case 0x7:
			{++instSetCounters[Strd_puw]; return Strd_puw; }
			default:
			{++instSetCounters[Unknown32]; return Unknown32; }
			}
		}
		else {
			instReg.DReg[0] = rt;
			instReg.DReg[1] = rt2;
			instReg.SReg[0] = rn;
			//	if(bits(puw,0)) instReg.DReg[3] = rn; no enough size for dest reg
			instType.isLoad = true;
			switch (puw) {
			case 0x1:
			{++instSetCounters[Ldrd_w]; return Ldrd_w; }
			case 0x3:
			{++instSetCounters[Ldrd_uw]; return Ldrd_uw; }
			case 0x4:
			{++instSetCounters[Ldrd_p]; return Ldrd_p; }
			case 0x5:
			{++instSetCounters[Ldrd_pw]; return Ldrd_pw; }
			case 0x6:
			{++instSetCounters[Ldrd_pu]; return Ldrd_pu; }
			case 0x7:
			{++instSetCounters[Ldrd_puw]; return Ldrd_puw; }
			default:
			{++instSetCounters[Unknown32]; return Unknown32; }
			}
		}
	}
}

inline Aarch32Set aarch32::Thumb32DataProcShiftReg(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const uint8_t op = bits(machInst, 24, 21);
	const bool s = (bits(machInst, 20) == 1);
	const IntRegIndex rn = (IntRegIndex)(uint8_t)bits(machInst, 19, 16);
	const IntRegIndex rd = (IntRegIndex)(uint8_t)bits(machInst, 11, 8);
	const IntRegIndex rm = (IntRegIndex)(uint8_t)bits(machInst, 3, 0);
	const uint8_t amt = (bits(machInst, 14, 12) << 2) | bits(machInst, 7, 6);
	const uint8_t type = bits(machInst, 5, 4);
	instReg.SReg[0] = rn;
	instReg.SReg[1] = rm;
	switch (op) {
	case 0x0:
		if (rd == 15) {
			if (s)
			{
				++instSetCounters[TstRegCc]; return TstRegCc;
			}
			else
			{
				++instSetCounters[TstReg]; return TstReg;
			}
		}
		else {
			if (s)
			{
				++instSetCounters[AndRegCc]; return AndRegCc;
			}
			else
			{
				++instSetCounters[AndReg]; return AndReg;
			}
		}
	case 0x1:{
		if (s)
		{
			++instSetCounters[BicRegCc]; return BicRegCc;
		}
		else
		{
			++instSetCounters[BicReg]; return BicReg;
		}
	}
	case 0x2:
		if (rn == 15) {
			if (s)
			{
				++instSetCounters[MovRegCc]; return MovRegCc;
			}
			else
			{
				++instSetCounters[MovReg]; return MovReg;
			}
		}
		else {
			if (s)
			{
				++instSetCounters[OrrRegCc]; return OrrRegCc;
			}
			else
			{
				++instSetCounters[OrrReg]; return OrrReg;
			}
		}
	case 0x3:
		if (rn == 15) {
			if (s)
			{
				++instSetCounters[MvnRegCc]; return MvnRegCc;
			}
			else
			{
				++instSetCounters[MvnReg]; return MvnReg;
			}
		}
		else {
			if (s)
			{
				++instSetCounters[OrnRegCc]; return OrnRegCc;
			}
			else
			{
				++instSetCounters[OrnReg]; return OrnReg;
			}
		}
	case 0x4:
		if (rd == 15) {
			if (s)
			{
				++instSetCounters[TeqRegCc]; return TeqRegCc;
			}
			else
			{
				++instSetCounters[TeqReg]; return TeqReg;
			}
		}
		else {
			if (s)
			{
				++instSetCounters[EorRegCc]; return EorRegCc;
			}
			else
			{
				++instSetCounters[EorReg]; return EorReg;
			}
		}
	case 0x6:{
		instReg.DReg[0] = rd;
		if (type) {
			{++instSetCounters[PkhtbReg]; return PkhtbReg; }
		}
		else {
			{++instSetCounters[PkhbtReg]; return PkhbtReg; }
		}
	}
	case 0x8:
		if (rd == 15) {
			if (s)
			{
				++instSetCounters[CmnRegCc]; return CmnRegCc;
			}
			else
			{
				++instSetCounters[CmnReg]; return CmnReg;
			}
		}
		else {
			if (s)
			{
				++instSetCounters[AddRegCc]; return AddRegCc;
			}
			else
			{
				++instSetCounters[AddReg]; return AddReg;
			}
		}
	case 0xa:
		if (s)
		{
			++instSetCounters[AdcRegCc]; return AdcRegCc;
		}
		else
		{
			++instSetCounters[AdcReg]; return AdcReg;
		}
	case 0xb:
		if (s)
		{
			++instSetCounters[SbcRegCc]; return SbcRegCc;
		}
		else
		{
			++instSetCounters[SbcReg]; return SbcReg;
		}
	case 0xd:
		if (rd == 15) {
			if (s)
			{
				++instSetCounters[CmpRegCc]; return CmpRegCc;
			}
			else
			{
				++instSetCounters[CmpReg]; return CmpReg;
			}
		}
		else {
			if (s)
			{
				++instSetCounters[SubRegCc]; return SubRegCc;
			}
			else
			{
				++instSetCounters[SubReg]; return SubReg;
			}
		}
	case 0xe:{
		if (s)
		{
			++instSetCounters[RsbRegCc]; return RsbRegCc;
		}
		else
		{
			++instSetCounters[RsbReg]; return RsbReg;
		}
	}
	default:
		{++instSetCounters[Unknown32]; return Unknown32; }
	}
}

inline Aarch32Set aarch32::Thumb32DataProcModImm(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const uint8_t op = bits(machInst, 24, 21);
	const bool s = (bits(machInst, 20) == 1);
	const IntRegIndex rn = (IntRegIndex)(uint8_t)bits(machInst, 19, 16);
	const IntRegIndex rd = (IntRegIndex)(uint8_t)bits(machInst, 11, 8);
	//const uint8_t ctrlImm = bits(machInst.instBits, 26) << 3 | bits(machInst, 14, 12);
	//const bool rotC = ctrlImm > 3;
	//const uint8_t dataImm = bits(machInst, 7, 0);
	//const uint8_t imm = modified_imm(ctrlImm, dataImm);
	switch (op) {
	case 0x0:
		if (rd == 15){
			instReg.SReg[0] = rn;
			if (s)
			{
				++instSetCounters[TstImmCc]; return TstImmCc;
			}
			else
			{
				++instSetCounters[TstImm]; return TstImm;
			}
		}
		else {
			instReg.DReg[0] = rd;
			instReg.SReg[0] = rn;
			if (s)
			{
				++instSetCounters[AndImmCc]; return AndImmCc;
			}
			else
			{
				++instSetCounters[AndImm]; return AndImm;
			}
		}
	case 0x1:{
		instReg.DReg[0] = rd;
		instReg.SReg[0] = rn;
		if (s)
		{
			++instSetCounters[BicImmCc]; return BicImmCc;
		}
		else
		{
			++instSetCounters[BicImm]; return BicImm;
		}
	}
	case 0x2:
		if (rn == 15){
			instReg.DReg[0] = rd;
			if (s)
			{
				++instSetCounters[MovImmCc]; return MovImmCc;
			}
			else
			{
				++instSetCounters[MovImm]; return MovImm;
			}
		}
		else {
			instReg.DReg[0] = rd;
			instReg.SReg[0] = rn;
			if (s)
			{
				++instSetCounters[OrrImmCc]; return OrrImmCc;
			}
			else
			{
				++instSetCounters[OrrImm]; return OrrImm;
			}
		}
	case 0x3:
		if (rn == 15){
			instReg.DReg[0] = rd;
			if (s)
			{
				++instSetCounters[MvnImmCc]; return MvnImmCc;
			}
			else
			{
				++instSetCounters[MvnImm]; return MvnImm;
			}
		}
		else {
			instReg.DReg[0] = rd;
			instReg.SReg[0] = rn;
			if (s)
			{
				++instSetCounters[OrnImmCc]; return OrnImmCc;
			}
			else
			{
				++instSetCounters[OrnImm]; return OrnImm;
			}
		}
	case 0x4:
		if (rd == 15){
			instReg.SReg[0] = rn;
			if (s)
			{
				++instSetCounters[TeqImmCc]; return TeqImmCc;
			}
			else
			{
				++instSetCounters[TeqImm]; return TeqImm;
			}
		}
		else {
			instReg.DReg[0] = rd;
			instReg.SReg[0] = rn;
			if (s)
			{
				++instSetCounters[EorImmCc]; return EorImmCc;
			}
			else
			{
				++instSetCounters[EorImm]; return EorImm;
			}
		}
	case 0x8:
		if (rd == 15){
			instReg.SReg[0] = rn;
			if (s)
			{
				++instSetCounters[CmnImmCc]; return CmnImmCc;
			}
			else
			{
				++instSetCounters[CmnImm]; return CmnImm;
			}
		}
		else {
			instReg.DReg[0] = rd;
			instReg.SReg[0] = rn;
			if (s)
			{
				++instSetCounters[AddImmCc]; return AddImmCc;
			}
			else
			{
				++instSetCounters[AddImm]; return AddImm;
			}
		}
	case 0xa:{
		instReg.DReg[0] = rd;
		instReg.SReg[0] = rn;
		if (s)
		{
			++instSetCounters[AdcImmCc]; return AdcImmCc;
		}
		else
		{
			++instSetCounters[AdcImm]; return AdcImm;
		}
	}
	case 0xb:{
		instReg.DReg[0] = rd;
		instReg.SReg[0] = rn;
		if (s)
		{
			++instSetCounters[SbcImmCc]; return SbcImmCc;
		}
		else
		{
			++instSetCounters[SbcImm]; return SbcImm;
		}
	}
	case 0xd:
		if (rd == 15){
			instReg.SReg[0] = rn;
			if (s)
			{
				++instSetCounters[CmpImmCc]; return CmpImmCc;
			}
			else
			{
				++instSetCounters[CmpImm]; return CmpImm;
			}
		}
		else {
			instReg.DReg[0] = rd;
			instReg.SReg[0] = rn;
			if (s)
			{
				++instSetCounters[SubImmCc]; return SubImmCc;
			}
			else
			{
				++instSetCounters[SubImm]; return SubImm;
			}
		}
	case 0xe:{
		instReg.DReg[0] = rd;
		instReg.SReg[0] = rn;
		if (s)
		{
			++instSetCounters[RsbImmCc]; return RsbImmCc;
		}
		else
		{
			++instSetCounters[RsbImm]; return RsbImm;
		}
	}
	default:
	{++instSetCounters[Unknown32]; return Unknown32; }
	}
}

inline Aarch32Set aarch32::Thumb32DataProcPlainBin(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const uint8_t op = bits(machInst, 24, 20);
	const IntRegIndex rn = (IntRegIndex)(uint8_t)bits(machInst, 19, 16);
	const IntRegIndex rd = (IntRegIndex)(uint8_t)bits(machInst, 11, 8);
	instReg.DReg[0] = rd;
	switch (op) {
	case 0x0:
	{
		const uint8_t imm = bits(machInst, 7, 0) | (bits(machInst, 14, 12) << 8) | (bits(machInst, 26) << 11);
		if (rn == 0xf) {
			{++instSetCounters[AdrImm]; return AdrImm; }
		}
		else {
			instReg.SReg[0] = rn;
			{++instSetCounters[AddImm]; return AddImm; }
		}
	}
	case 0x4:
	{
		const uint8_t imm = bits(machInst, 7, 0) | (bits(machInst, 14, 12) << 8) | (bits(machInst, 26) << 11) | (bits(machInst, 19, 16) << 12);
		{++instSetCounters[MovImm]; return MovImm; }
	}
	case 0xa:
	{
		const uint8_t imm = bits(machInst, 7, 0) | (bits(machInst, 14, 12) << 8) | (bits(machInst, 26) << 11);
		if (rn == 0xf) {
			{++instSetCounters[AdrImm]; return AdrImm; }
		}
		else {
			instReg.SReg[0] = rn;
			{++instSetCounters[SubImm]; return SubImm; }
		}
	}
	case 0xc:
	{
		const uint8_t imm = bits(machInst, 7, 0) | (bits(machInst, 14, 12) << 8) | (bits(machInst, 26) << 11) | (bits(machInst, 19, 16) << 12);
		{++instSetCounters[MovtImm]; return MovtImm; }
	}
	case 0x12:
		if (!(bits(machInst, 14, 12) || bits(machInst, 7, 6))) {
			const uint8_t satImm = bits(machInst, 4, 0);
			{++instSetCounters[Ssat16]; return Ssat16; }
		}
		// Fall through on purpose...
	case 0x10:
	{
		const uint8_t satImm = bits(machInst, 4, 0);
		const uint8_t imm = bits(machInst, 7, 6) | (bits(machInst, 14, 12) << 2);
		const ArmShiftType type = (ArmShiftType)(uint8_t)bits(machInst, 21, 20);
		instReg.SReg[0] = rn;
		{++instSetCounters[Ssat]; return Ssat; }
	}
	case 0x14:
	{
		const uint8_t lsb = bits(machInst, 7, 6) | (bits(machInst, 14, 12) << 2);
		const uint8_t msb = lsb + bits(machInst, 4, 0);
		instReg.SReg[0] = rn;
		{++instSetCounters[Sbfx]; return Sbfx; }
	}
	case 0x16:
	{
		const uint8_t lsb = bits(machInst, 7, 6) | (bits(machInst, 14, 12) << 2);
		const uint8_t msb = bits(machInst, 4, 0);
		if (rn == 0xf) {
			{++instSetCounters[Bfc]; return Bfc; }
		}
		else {
			instReg.SReg[0] = rn;
			{++instSetCounters[Bfi]; return Bfi; }
		}
	}
	case 0x1a:
		if (!(bits(machInst, 14, 12) || bits(machInst, 7, 6))) {
			const uint8_t satImm = bits(machInst, 4, 0);
			instReg.SReg[0] = rn;
			{++instSetCounters[Usat16]; return Usat16; }
		}
		// Fall through on purpose...
	case 0x18:
	{
		const uint8_t satImm = bits(machInst, 4, 0);
		const uint8_t imm = bits(machInst, 7, 6) | (bits(machInst, 14, 12) << 2);
		const ArmShiftType type = (ArmShiftType)(uint8_t)bits(machInst, 21, 20);
		instReg.SReg[0] = rn;
		{++instSetCounters[Usat]; return Usat; }
	}
	case 0x1c:
	{
		const uint8_t lsb = bits(machInst, 7, 6) | (bits(machInst, 14, 12) << 2);
		const uint8_t msb = lsb + bits(machInst, 4, 0);
		instReg.SReg[0] = rn;
		{++instSetCounters[Ubfx]; return Ubfx; }
	}
	default:
	{++instSetCounters[Unknown32]; return Unknown32; }
	}
}

inline Aarch32Set aarch32::Thumb32BranchesAndMiscCtrl(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const uint8_t op = bits(machInst, 26, 20);
	const uint8_t op1 = bits(machInst, 14, 12);
	switch (op1 & 0x5) {
	case 0x0:
		if (op == 127) {
			if (op1 & 0x2) {
				// Permanently undefined.
				{++instSetCounters[Unknown32]; return Unknown32; }
			}
			else {
				{++instSetCounters[Smc]; return Smc; }
			}
		}
		else if ((op & 0x38) != 0x38) {
			const uint8_t s = bits(machInst, 26);
			const uint8_t j1 = bits(machInst, 13);
			const uint8_t j2 = bits(machInst, 11);
			const uint8_t imm6 = bits(machInst, 21, 16);
			const uint8_t imm11 = bits(machInst, 10, 0);
			const int32_t imm = sext<21>((s << 20) | (j2 << 19) | (j1 << 18) | (imm6 << 12) | (imm11 << 1));
			{++instSetCounters[B]; return B; }
		}
		else {
			// HIGH: 12-11=10, LOW: 15-14=00, 12=0
			switch (op) {
			case 0x38:
			case 0x39:
			{
				const IntRegIndex rn = (IntRegIndex)(uint8_t)bits(machInst, 19, 16);
				const uint8_t byteMask = bits(machInst, 11, 8);
				const bool r = bits(machInst, 20) ? true : false;
				instReg.SReg[0] = rn;
				if (bits(machInst, 5)) {
					const uint8_t sysM = (bits(machInst, 4) << 4) | byteMask;
					{++instSetCounters[MsrBankedReg]; return MsrBankedReg; }
				}
				else {
					if (r) {
						{++instSetCounters[MsrSpsrReg]; return MsrSpsrReg; }
					}
					else {
						{++instSetCounters[MsrCpsrReg]; return MsrCpsrReg; }
					}
				}
			}
			case 0x3a:
			{
				const uint8_t op1 = bits(machInst, 10, 8);
				const uint8_t op2 = bits(machInst, 7, 0);
				if (op1 != 0) {
					const bool enable = bits(machInst, 10, 9) == 0x2;
					const uint8_t mods = bits(machInst, 8, 0) | ((enable ? 1 : 0) << 9);
					{++instSetCounters[Cps]; return Cps; }
				}
				else if ((op2 & 0xf0) == 0xf0) {
					{++instSetCounters[Dbg]; return Dbg; }
				}
				else {
					switch (op2) {
					case 0x0:
					{++instSetCounters[NopInst]; return NopInst; }
					case 0x1:
					{++instSetCounters[YieldInst]; return YieldInst; }
					case 0x2:
					{++instSetCounters[WfeInst]; return WfeInst; }
					case 0x3:
					{++instSetCounters[WfiInst]; return WfiInst; }
					case 0x4:
					{++instSetCounters[SevInst]; return SevInst; }
					default:
						break;
					}
				}
				break;
			}
			case 0x3b:
			{
				const uint8_t op = bits(machInst, 7, 4);
				switch (op) {
				case 0x0:
				{++instSetCounters[Leavex]; return Leavex; }
				case 0x1:
				{++instSetCounters[Enterx]; return Enterx; }
				case 0x2:
				{++instSetCounters[Clrex]; return Clrex; }
				case 0x4:
				{instType.isSerial = true; ++instSetCounters[Dsb]; return Dsb; }
				case 0x5:
				{instType.isSerial = true; ++instSetCounters[Dmb]; return Dmb; }
				case 0x6:
				{instType.isSerial = true; ++instSetCounters[Isb]; return Isb; }
				default:
					break;
				}
				break;
			}
			case 0x3c:
			{
				{instType.isBranch = true; ++instSetCounters[BxjReg]; return BxjReg; }
			}
			case 0x3d:
			{
				const uint8_t imm32 = bits(machInst, 7, 0);
				if (imm32 == 0) {
					{instType.isBranch = true; ++instSetCounters[Eret]; return Eret; }
				}
				else {
					{++instSetCounters[SubsImmPclr]; return SubsImmPclr; }
				}
			}
			case 0x3e:
			case 0x3f:
			{

				const IntRegIndex rd = (IntRegIndex)(uint8_t)bits(machInst, 11, 8);
				const bool r = bits(machInst, 20) ? true : false;
				instReg.DReg[0] = rd;
				if (bits(machInst, 5)) {
					const uint8_t sysM = (bits(machInst, 4) << 4) | bits(machInst, 11, 8);
					{++instSetCounters[MrsBankedReg]; return MrsBankedReg; }
				}
				else {
					if (r) {
						{++instSetCounters[MrsSpsr]; return MrsSpsr; }
					}
					else {
						{++instSetCounters[MrsCpsr]; return MrsCpsr; }
					}
				}
			}
			case 0xfe:
			{
				uint8_t imm16 = (bits(machInst, 19, 16) << 12) | (bits(machInst, 11, 0) << 0);
				{instType.isBranch = true; ++instSetCounters[Hvc]; return Hvc; }
			}
			}
			break;
		}
	case 0x1:
	{
		const uint8_t s = bits(machInst, 26);
		const uint8_t i1 = !(bits(machInst, 13) ^ s);
		const uint8_t i2 = !(bits(machInst, 11) ^ s);
		const uint8_t imm10 = bits(machInst, 25, 16);
		const uint8_t imm11 = bits(machInst, 10, 0);
		const int32_t imm = sext<25>((s << 24) | (i1 << 23) | (i2 << 22) | (imm10 << 12) | (imm11 << 1));
		{instType.isBranch = true; instType.isCc = true; ++instSetCounters[B]; return B; }
	}
	case 0x4:
	{
		if (bits(machInst, 0) == 1) {
			{++instSetCounters[Unknown32]; return Unknown32; }
		}
		const uint8_t s = bits(machInst, 26);
		const uint8_t i1 = !(bits(machInst, 13) ^ s);
		const uint8_t i2 = !(bits(machInst, 11) ^ s);
		const uint8_t imm10h = bits(machInst, 25, 16);
		const uint8_t imm10l = bits(machInst, 10, 1);
		const int32_t imm = sext<25>((s << 24) | (i1 << 23) | (i2 << 22) | (imm10h << 12) | (imm10l << 2));
		{instType.isBranch = true; ++instSetCounters[BlxImm]; return BlxImm; }
	}
	case 0x5:
	{
		const uint8_t s = bits(machInst, 26);
		const uint8_t i1 = !(bits(machInst, 13) ^ s);
		const uint8_t i2 = !(bits(machInst, 11) ^ s);
		const uint8_t imm10 = bits(machInst, 25, 16);
		const uint8_t imm11 = bits(machInst, 10, 0);
		const int32_t imm = sext<25>((s << 24) | (i1 << 23) | (i2 << 22) | (imm10 << 12) | (imm11 << 1));
		{instType.isBranch = true; ++instSetCounters[Bl]; return Bl; }
	}
	default:
		break;
	}
	{++instSetCounters[Unknown32]; return Unknown32; }
}

inline Aarch32Set aarch32::Thumb32StoreSingle(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	uint8_t op1 = bits(machInst, 23, 21);
	uint8_t op2 = bits(machInst, 11, 6);
	bool op2Puw = ((op2 & 0x24) == 0x24 || (op2 & 0x3c) == 0x30);
	const IntRegIndex rt = (IntRegIndex)(uint8_t)bits(machInst, 15, 12);
	const IntRegIndex rn = (IntRegIndex)(uint8_t)bits(machInst, 19, 16);
	uint32_t puw = bits(machInst, 10, 8);
	if (rn == 0xf) { ++instSetCounters[Unknown32]; return Unknown32; }
	instReg.SReg[0] = rt;
	instReg.SReg[1] = rn;
	instType.isStore = true;
	if (op1 == 4)
	{
		++instSetCounters[Strb_imm]; return Strb_imm;
	}
	else if (op1 == 0 && op2Puw) {
		if (bits(puw, 0)) instReg.DReg[0] = rn;
		switch (puw) {
		case 0:
		case 2:
		{++instSetCounters[Unknown32]; return Unknown32; }
		case 1:
		{++instSetCounters[Strbimm_w]; return Strbimm_w; }
		case 3:
		{++instSetCounters[Strbimm_uw]; return Strbimm_uw; }
		case 4:
		{++instSetCounters[Strbimm_p]; return Strbimm_p; }
		case 5:
		{++instSetCounters[Strbimm_pw]; return Strbimm_pw; }
		case 6:
		{++instSetCounters[Strbimm_pu]; return Strbimm_pu; }
		case 7:
		{++instSetCounters[Strbimm_puw]; return Strbimm_puw; }
		}
	}
	else if (op1 == 0 && ((op2 & 0x3c) == 0x38)) {
		{++instSetCounters[Strbt]; return Strbt; }
	}
	else if (op1 == 0 && op2 == 0) {
		{++instSetCounters[Strb_reg]; return Strb_reg; }
	}
	else if (op1 == 5) {
		{++instSetCounters[Strh_imm]; return Strh_imm; }
	}
	else if (op1 == 1 && op2Puw) {
		if (bits(puw, 0)) instReg.DReg[0] = rn;
		switch (puw) {
		case 0:
		case 2:
		{++instSetCounters[Unknown32]; return Unknown32; }
		case 1:
		{++instSetCounters[Strhimm_w]; return Strhimm_w; }
		case 3:
		{++instSetCounters[Strhimm_uw]; return Strhimm_uw; }
		case 4:
		{++instSetCounters[Strhimm_p]; return Strhimm_p; }
		case 5:
		{++instSetCounters[Strhimm_pw]; return Strhimm_pw; }
		case 6:
		{++instSetCounters[Strhimm_pu]; return Strhimm_pu; }
		case 7:
		{++instSetCounters[Strhimm_puw]; return Strhimm_puw; }
		}
	}
	else if (op1 == 1 && ((op2 & 0x3c) == 0x38)) {
		{++instSetCounters[Strht]; return Strht; }
	}
	else if (op1 == 1 && op2 == 0) {
		{++instSetCounters[Strh_reg]; return Strh_reg; }
	}
	else if (op1 == 6) {
		{++instSetCounters[Str_imm]; return Str_imm; }
	}
	else if (op1 == 2 && op2Puw) {
		if (bits(puw, 0)) instReg.DReg[0] = rn;
		switch (puw) {
		case 0:
		case 2:
		{++instSetCounters[Unknown32]; return Unknown32; }
		case 1:
		{++instSetCounters[Strimm_w]; return Strimm_w; }
		case 3:
		{++instSetCounters[Strimm_uw]; return Strimm_uw; }
		case 4:
		{++instSetCounters[Strimm_p]; return Strimm_p; }
		case 5:
		{++instSetCounters[Strimm_pw]; return Strimm_pw; }
		case 6:
		{++instSetCounters[Strimm_pu]; return Strimm_pu; }
		case 7:
		{++instSetCounters[Strimm_puw]; return Strimm_puw; }
		}
	}
	else if (op1 == 2 && ((op2 & 0x3c) == 0x38)) {
		{++instSetCounters[Strt]; return Strt; }
	}
	else if (op1 == 2 && op2 == 0) {
		{++instSetCounters[Str_reg]; return Str_reg; }
	}
	else {
		{++instSetCounters[Unknown32]; return Unknown32; }
	}
	{++instSetCounters[Unknown32]; return Unknown32; }
}

inline Aarch32Set aarch32::Thumb32LoadWord(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	uint8_t op1 = bits(machInst, 24, 23);
	const IntRegIndex rt = (IntRegIndex)(uint8_t)bits(machInst, 15, 12);
	const IntRegIndex rn = (IntRegIndex)(uint8_t)bits(machInst, 19, 16);
	instType.isLoad = true;
	if (bits(op1, 1) == 0) {
		uint8_t op2 = bits(machInst, 11, 6);
		instReg.DReg[0] = rt;
		if (bits(machInst, 19, 16) == 0xF) {
			if (bits(machInst, 23))
			{
				++instSetCounters[Ldrliteral_u]; return Ldrliteral_u;
			}
			else
			{
				++instSetCounters[Ldrliteral]; return Ldrliteral;
			}
		}
		else if (op1 == 0x1){
			instReg.SReg[0] = rn;
			{++instSetCounters[Ldrimm_pu]; return Ldrimm_pu; }
		}
		else if (op2 == 0){
			instReg.SReg[0] = rn;
			{++instSetCounters[Ldrreg]; return Ldrreg; }
		}
		else if ((op2 & 0x3c) == 0x38){
			instReg.SReg[0] = rn;
			{++instSetCounters[Ldrt]; return Ldrt; }
		}
		else if ((op2 & 0x3c) == 0x30 || (op2 & 0x24) == 0x24) {
			uint8_t puw = bits(machInst, 10, 8);
			uint8_t imm = 0;// IMMED_7_0;
			instReg.SReg[0] = rn;
			if (bits(puw, 0)) instReg.DReg[0] = rn;
			switch (puw) {
			case 0:
			case 2: {++instSetCounters[Unknown32]; return Unknown32; }
			case 1:
			{++instSetCounters[Ldrimm_w]; return Ldrimm_w; }
			case 3:
			{++instSetCounters[Ldrimm_uw]; return Ldrimm_uw; }
			case 4:
			{++instSetCounters[Ldrimm_p]; return Ldrimm_p; }
			case 5:
			{++instSetCounters[Ldrimm_pw]; return Ldrimm_pw; }
			case 6:
			{++instSetCounters[Ldrimm_pu]; return Ldrimm_pu; }
			case 7:
			{++instSetCounters[Ldrimm_puw]; return Ldrimm_puw; }
			}
		}
	}
	else {
		{++instSetCounters[Unknown32]; return Unknown32; }
	}
	{++instSetCounters[Unknown32]; return Unknown32; }
}

inline Aarch32Set aarch32::Thumb32MulMulAccAndAbsDiff(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const uint8_t op1 = bits(machInst, 22, 20);
	const uint8_t op2 = bits(machInst, 5, 4);
	const IntRegIndex ra = (IntRegIndex)(uint8_t)bits(machInst, 15, 12);
	const IntRegIndex rn = (IntRegIndex)(uint8_t)bits(machInst, 19, 16);
	const IntRegIndex rd = (IntRegIndex)(uint8_t)bits(machInst, 11, 8);
	const IntRegIndex rm = (IntRegIndex)(uint8_t)bits(machInst, 3, 0);
	if (op1 != 0x1 && bits(op2, 1) != 0) {
		{++instSetCounters[Unknown32]; return Unknown32; }
	}
	instReg.DReg[0] = rd;
	instReg.SReg[0] = rn;
	instReg.SReg[1] = rm;
	if (ra != 0xf)
		instReg.SReg[2] = ra;
	switch (op1) {
	case 0x0:
		if (op2 == 0) {
			if (ra == 0xf) {
				{++instSetCounters[Mul]; return Mul; }
			}
			else {
				{++instSetCounters[Mla]; return Mla; }
			}
		}
		else {
			{++instSetCounters[Mls]; return Mls; }
		}
	case 0x1:
		if (ra == 0xf) {
			switch (bits(machInst, 5, 4)) {
			case 0x0:
			{++instSetCounters[Smulbb]; return Smulbb; }
			case 0x1:
			{++instSetCounters[Smulbt]; return Smulbt; }
			case 0x2:
			{++instSetCounters[Smultb]; return Smultb; }
			case 0x3:
			{++instSetCounters[Smultt]; return Smultt; }
			}
		}
		else {
			switch (bits(machInst, 5, 4)) {
			case 0x0:
			{++instSetCounters[SmlabbCc]; return SmlabbCc; }
			case 0x1:
			{++instSetCounters[SmlabtCc]; return SmlabtCc; }
			case 0x2:
			{++instSetCounters[SmlatbCc]; return SmlatbCc; }
			case 0x3:
			{++instSetCounters[SmlattCc]; return SmlattCc; }
			}
		}
	case 0x2:
		if (ra == 0xf) {
			if (bits(machInst, 4)) {
				{++instSetCounters[SmuadxCc]; return SmuadxCc; }
			}
			else {
				{++instSetCounters[SmuadCc]; return SmuadCc; }
			}
		}
		else {
			if (bits(machInst, 4)) {
				{++instSetCounters[SmladxCc]; return SmladxCc; }
			}
			else {
				{++instSetCounters[SmladCc]; return SmladCc; }
			}
		}
	case 0x3:
		if (ra == 0xf) {
			if (bits(machInst, 4)) {
				{++instSetCounters[Smulwt]; return Smulwt; }
			}
			else {
				{++instSetCounters[Smulwb]; return Smulwb; }
			}
		}
		else {
			if (bits(machInst, 4)) {
				{++instSetCounters[SmlawtCc]; return SmlawtCc; }
			}
			else {
				{++instSetCounters[SmlawbCc]; return SmlawbCc; }
			}
		}
	case 0x4:
		if (ra == 0xf) {
			if (bits(machInst, 4)) {
				{++instSetCounters[Smusdx]; return Smusdx; }
			}
			else {
				{++instSetCounters[Smusd]; return Smusd; }
			}
		}
		else {
			if (bits(machInst, 4)) {
				{++instSetCounters[SmlsdxCc]; return SmlsdxCc; }
			}
			else {
				{++instSetCounters[SmlsdCc]; return SmlsdCc; }
			}
		}
	case 0x5:
		if (ra == 0xf) {
			if (bits(machInst, 4)) {
				{++instSetCounters[Smmulr]; return Smmulr; }
			}
			else {
				{++instSetCounters[Smmul]; return Smmul; }
			}
		}
		else {
			if (bits(machInst, 4)) {
				{++instSetCounters[Smmlar]; return Smmlar; }
			}
			else {
				{++instSetCounters[Smmla]; return Smmla; }
			}
		}
	case 0x6:
		if (bits(machInst, 4)) {
			{++instSetCounters[Smmlsr]; return Smmlsr; }
		}
		else {
			{++instSetCounters[Smmls]; return Smmls; }
		}
	case 0x7:
		if (op2 != 0x0) {
			{++instSetCounters[Unknown32]; return Unknown32; }
		}
		else if (ra == 0xf) {
			{++instSetCounters[Usad8]; return Usad8; }
		}
		else {
			{++instSetCounters[Usada8]; return Usada8; }
		}
	}
	{++instSetCounters[Unknown32]; return Unknown32; }
}

inline Aarch32Set aarch32::Thumb32LongMulMulAccAndDiv(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const uint8_t op1 = bits(machInst, 22, 20);
	const uint8_t op2 = bits(machInst, 7, 4);
	const IntRegIndex rn = (IntRegIndex)(uint8_t)bits(machInst, 19, 16);
	const IntRegIndex rdlo = (IntRegIndex)(uint8_t)bits(machInst, 15, 12);
	const IntRegIndex rdhi = (IntRegIndex)(uint8_t)bits(machInst, 11, 8);
	const IntRegIndex rm = (IntRegIndex)(uint8_t)bits(machInst, 3, 0);
	instReg.DReg[0] = rdhi;
	if (op2 != 0xf) instReg.DReg[1] = rdlo;
	instReg.SReg[0] = rm;
	instReg.SReg[1] = rn;
	switch (op1) {
	case 0x0:
		if (op2 == 0x0) {
			{++instSetCounters[Smull]; return Smull; }
		}
		break;
	case 0x1:
		if (op2 == 0xf) {
			{++instSetCounters[Sdiv]; return Sdiv; }
		}
		break;
	case 0x2:
		if (op2 == 0x0) {
			{++instSetCounters[Umull]; return Umull; }
		}
		break;
	case 0x3:
		if (op2 == 0xf) {
			{++instSetCounters[Udiv]; return Udiv; }
		}
		break;
	case 0x4:
		if (op2 == 0) {
			{++instSetCounters[Smlal]; return Smlal; }
		}
		else if (bits(op2, 3, 2) == 0x2) {
			switch (bits(machInst, 5, 4)) {
			case 0x0:
			{++instSetCounters[Smlalbb]; return Smlalbb; }
			case 0x1:
			{++instSetCounters[Smlalbt]; return Smlalbt; }
			case 0x2:
			{++instSetCounters[Smlaltb]; return Smlaltb; }
			case 0x3:
			{++instSetCounters[Smlaltt]; return Smlaltt; }
			}
		}
		else if (bits(op2, 3, 1) == 0x6) {
			if (bits(machInst, 4)) {
				{++instSetCounters[Smlaldx]; return Smlaldx; }
			}
			else {
				{++instSetCounters[Smlald]; return Smlald; }
			}
		}
		break;
	case 0x5:
		if (bits(op2, 3, 1) == 0x6) {
			if (bits(machInst, 4)) {
				{++instSetCounters[Smlsldx]; return Smlsldx; }
			}
			else {
				{++instSetCounters[Smlsld]; return Smlsld; }
			}
		}
		break;
	case 0x6:
		if (op2 == 0) {
			{++instSetCounters[Umlal]; return Umlal; }
		}
		else if (op2 == 0x6) {
			{++instSetCounters[Umaal]; return Umaal; }
		}
		break;
	}
	{++instSetCounters[Unknown32]; return Unknown32; }
}

inline Aarch32Set aarch32::Thumb32DataProcReg(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const uint8_t op1 = bits(machInst, 23, 20);
	const IntRegIndex rn = (IntRegIndex)(uint8_t)bits(machInst, 19, 16);
	const uint8_t op2 = bits(machInst, 7, 4);
	// by shen 11/23
	//if (bits(machInst, 15, 12) != 0xf) {
	//	{++instSetCounters[Unknown32]; return Unknown32; }
	//}
	if (bits(op1, 3) != 1) {
		if (op2 == 0) {
			IntRegIndex rd = (IntRegIndex)(uint8_t)bits(machInst, 11, 8);
			IntRegIndex rs = (IntRegIndex)(uint8_t)bits(machInst, 3, 0);
			instReg.DReg[0] = rd;
			instReg.SReg[0] = rs;
			instReg.SReg[1] = rn;
			switch (bits(op1, 2, 0)) {
			case 0x0:
			{++instSetCounters[MovRegRegLSL]; return MovRegRegLSL; }
			case 0x1:
			{++instSetCounters[MovRegRegLSLCc]; return MovRegRegLSLCc; }
			case 0x2:
			{++instSetCounters[MovRegRegLSR]; return MovRegRegLSR; }
			case 0x3:
			{++instSetCounters[MovRegRegLSRCc]; return MovRegRegLSRCc; }
			case 0x4:
			{++instSetCounters[MovRegRegASR]; return MovRegRegASR; }
			case 0x5:
			{++instSetCounters[MovRegRegASRCc]; return MovRegRegASRCc; }
			case 0x6:
			{++instSetCounters[MovRegRegROR]; return MovRegRegROR; }
			case 0x7:
			{++instSetCounters[MovRegRegRORCc]; return MovRegRegRORCc; }
			}
		}
		else if (bits(op2, 3) == 0) {
			{++instSetCounters[Unknown32]; return Unknown32; }
		}
		else {
			const IntRegIndex rd = (IntRegIndex)(uint8_t)bits(machInst, 11, 8);
			const IntRegIndex rm = (IntRegIndex)(uint8_t)bits(machInst, 3, 0);
			const uint8_t rotation = (uint8_t)bits(machInst, 5, 4) << 3;
			instReg.DReg[0] = rd;
			instReg.SReg[0] = rm;
			if (rn != 0xf) instReg.SReg[1] = rn;
			switch (bits(op1, 2, 0)) {
			case 0x0:
				if (rn == 0xf) {
					{++instSetCounters[Sxth]; return Sxth; }
				}
				else {
					{++instSetCounters[Sxtah]; return Sxtah; }
				}
			case 0x1:
				if (rn == 0xf) {
					{++instSetCounters[Uxth]; return Uxth; }
				}
				else {
					{++instSetCounters[Uxtah]; return Uxtah; }
				}
			case 0x2:
				if (rn == 0xf) {
					{++instSetCounters[Sxtb16]; return Sxtb16; }
				}
				else {
					{++instSetCounters[Sxtab16]; return Sxtab16; }
				}
			case 0x3:
				if (rn == 0xf) {
					{++instSetCounters[Uxtb16]; return Uxtb16; }
				}
				else {
					{++instSetCounters[Uxtab16]; return Uxtab16; }
				}
			case 0x4:
				if (rn == 0xf) {
					{++instSetCounters[Sxtb]; return Sxtb; }
				}
				else {
					{++instSetCounters[Sxtab]; return Sxtab; }
				}
			case 0x5:
				if (rn == 0xf) {
					{++instSetCounters[Uxtb]; return Uxtb; }
				}
				else {
					{++instSetCounters[Uxtab]; return Uxtab; }
				}
			default:
			{++instSetCounters[Unknown32]; return Unknown32; }
			}
		}
	}
	else {
		if (bits(op2, 3) == 0) {
			const IntRegIndex rd = (IntRegIndex)(uint8_t)bits(machInst, 11, 8);
			const IntRegIndex rm = (IntRegIndex)(uint8_t)bits(machInst, 3, 0);
			instReg.DReg[0] = rd;
			instReg.SReg[0] = rm;
			instReg.SReg[1] = rn;
			if (bits(op2, 2) == 0x0) {
				const uint8_t op1 = bits(machInst, 22, 20);
				const uint8_t op2 = bits(machInst, 5, 4);
				switch (op2) {
				case 0x0:
					switch (op1) {
					case 0x1:
					{++instSetCounters[Sadd16RegCc]; return Sadd16RegCc; }
					case 0x2:
					{++instSetCounters[SasxRegCc]; return SasxRegCc; }
					case 0x6:
					{++instSetCounters[SsaxRegCc]; return SsaxRegCc; }
					case 0x5:
					{++instSetCounters[Ssub16RegCc]; return Ssub16RegCc; }
					case 0x0:
					{++instSetCounters[Sadd8RegCc]; return Sadd8RegCc; }
					case 0x4:
					{++instSetCounters[Ssub8RegCc]; return Ssub8RegCc; }
					}
					break;
				case 0x1:
					switch (op1) {
					case 0x1:
					{++instSetCounters[Qadd16Reg]; return Qadd16Reg; }
					case 0x2:
					{++instSetCounters[QasxReg]; return QasxReg; }
					case 0x6:
					{++instSetCounters[QsaxReg]; return QsaxReg; }
					case 0x5:
					{++instSetCounters[Qsub16Reg]; return Qsub16Reg; }
					case 0x0:
					{++instSetCounters[Qadd8Reg]; return Qadd8Reg; }
					case 0x4:
					{++instSetCounters[Qsub8Reg]; return Qsub8Reg; }
					}
					break;
				case 0x2:
					switch (op1) {
					case 0x1:
					{++instSetCounters[Shadd16Reg]; return Shadd16Reg; }
					case 0x2:
					{++instSetCounters[ShasxReg]; return ShasxReg; }
					case 0x6:
					{++instSetCounters[ShsaxReg]; return ShsaxReg; }
					case 0x5:
					{++instSetCounters[Shsub16Reg]; return Shsub16Reg; }
					case 0x0:
					{++instSetCounters[Shadd8Reg]; return Shadd8Reg; }
					case 0x4:
					{++instSetCounters[Shsub8Reg]; return Shsub8Reg; }
					}
					break;
				}
			}
			else {
				const uint8_t op1 = bits(machInst, 22, 20);
				const uint8_t op2 = bits(machInst, 5, 4);
				switch (op2) {
				case 0x0:
					switch (op1) {
					case 0x1:
					{++instSetCounters[Uadd16RegCc]; return Uadd16RegCc; }
					case 0x2:
					{++instSetCounters[UasxRegCc]; return UasxRegCc; }
					case 0x6:
					{++instSetCounters[UsaxRegCc]; return UsaxRegCc; }
					case 0x5:
					{++instSetCounters[Usub16RegCc]; return Usub16RegCc; }
					case 0x0:
					{++instSetCounters[Uadd8RegCc]; return Uadd8RegCc; }
					case 0x4:
					{++instSetCounters[Usub8RegCc]; return Usub8RegCc; }
					}
					break;
				case 0x1:
					switch (op1) {
					case 0x1:
					{++instSetCounters[Uqadd16Reg]; return Uqadd16Reg; }
					case 0x2:
					{++instSetCounters[UqasxReg]; return UqasxReg; }
					case 0x6:
					{++instSetCounters[UqsaxReg]; return UqsaxReg; }
					case 0x5:
					{++instSetCounters[Uqsub16Reg]; return Uqsub16Reg; }
					case 0x0:
					{++instSetCounters[Uqadd8Reg]; return Uqadd8Reg; }
					case 0x4:
					{++instSetCounters[Uqsub8Reg]; return Uqsub8Reg; }
					}
					break;
				case 0x2:
					switch (op1) {
					case 0x1:
					{++instSetCounters[Uhadd16Reg]; return Uhadd16Reg; }
					case 0x2:
					{++instSetCounters[UhasxReg]; return UhasxReg; }
					case 0x6:
					{++instSetCounters[UhsaxReg]; return UhsaxReg; }
					case 0x5:
					{++instSetCounters[Uhsub16Reg]; return Uhsub16Reg; }
					case 0x0:
					{++instSetCounters[Uhadd8Reg]; return Uhadd8Reg; }
					case 0x4:
					{++instSetCounters[Uhsub8Reg]; return Uhsub8Reg; }
					}
					break;
				}
			}
		}
		else if (bits(op1, 3, 2) == 0x2 && bits(op2, 3, 2) == 0x2) {
			const uint8_t op1 = bits(machInst, 21, 20);
			const uint8_t op2 = bits(machInst, 5, 4);
			const IntRegIndex rd = (IntRegIndex)(uint8_t)bits(machInst, 11, 8);
			const IntRegIndex rm = (IntRegIndex)(uint8_t)bits(machInst, 3, 0);
			instReg.DReg[0] = rd;
			switch (op1) {
			case 0x0:{
				instReg.SReg[0] = rm;
				instReg.SReg[1] = rn;
				switch (op2) {
				case 0x0:
				{++instSetCounters[QaddRegCc]; return QaddRegCc; }
				case 0x1:
				{++instSetCounters[QdaddRegCc]; return QdaddRegCc; }
				case 0x2:
				{++instSetCounters[QsubRegCc]; return QsubRegCc; }
				case 0x3:
				{++instSetCounters[QdsubRegCc]; return QdsubRegCc; }
				}
			}
					 break;
			case 0x1:
				switch (op2) {
				case 0x0:{
					instReg.SReg[0] = rn;
					{++instSetCounters[Rev]; return Rev; }
				}
				case 0x1:{
					instReg.SReg[0] = rn;
					{++instSetCounters[Rev16]; return Rev16; }
				}
				case 0x2:{
					instReg.SReg[0] = rm;
					{++instSetCounters[Rbit]; return Rbit; }
				}
				case 0x3:{
					instReg.SReg[0] = rn;
					{++instSetCounters[Revsh]; return Revsh; }
				}
				}
				break;
			case 0x2:
				if (op2 == 0) {
					instReg.SReg[0] = rm;
					instReg.SReg[1] = rn;
					{++instSetCounters[Sel]; return Sel; }
				}
				break;
			case 0x3:
				if (op2 == 0) {
					instReg.SReg[0] = rm;
					{++instSetCounters[Clz]; return Clz; }
				}
			}
		}
		{++instSetCounters[Unknown32]; return Unknown32; }
	}
	{++instSetCounters[Unknown32]; return Unknown32; }
}
// com 
inline Aarch32Set aarch32::ArmMacroMem(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const IntRegIndex rn = (IntRegIndex)(uint8_t)bits(machInst, 19, 16);
	const bool wback = bits(machInst, 21) ? true : false;
	const bool load = bits(machInst, 20) ? true : false;
	if (wback) instReg.DReg[0] = rn;
	instReg.SReg[0] = rn;
	if (load){
		instType.isLoad = true;
		{++instSetCounters[LdmStmLd]; return LdmStmLd; }
	}
	else{
		instType.isStore = true;
		{++instSetCounters[LdmStmSt]; return LdmStmSt; }
	}
}
// com
inline Aarch32Set aarch32::M5ops(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const uint8_t m5func = bits(machInst, 23, 16);
	switch (m5func) {
	case 0x00: {++instSetCounters[Arm]; return Arm; }
	case 0x01: {++instSetCounters[Quiesce]; return Quiesce; }
	case 0x02: {++instSetCounters[QuiesceNs]; return QuiesceNs; }
	case 0x03: {++instSetCounters[QuiesceCycles]; return QuiesceCycles; }
	case 0x04: {++instSetCounters[QuiesceTime]; return QuiesceTime; }
	case 0x07: {++instSetCounters[Rpns]; return Rpns; }
	case 0x09: {++instSetCounters[WakeCPU]; return WakeCPU; }
	case 0x10: {++instSetCounters[Deprecated_ivlb]; return Deprecated_ivlb; }
	case 0x11: {++instSetCounters[Deprecated_ivle]; return Deprecated_ivle; }
	case 0x20: {++instSetCounters[Deprecated_exit]; return Deprecated_exit; }
	case 0x21: {++instSetCounters[M5exit]; return M5exit; }
	case 0x22: {++instSetCounters[M5fail]; return M5fail; }
	case 0x31: {++instSetCounters[Loadsymbol]; return Loadsymbol; }
	case 0x30: {++instSetCounters[Initparam]; return Initparam; }
	case 0x40: {++instSetCounters[Resetstats]; return Resetstats; }
	case 0x41: {++instSetCounters[Dumpstats]; return Dumpstats; }
	case 0x42: {++instSetCounters[Dumpresetstats]; return Dumpresetstats; }
	case 0x43: {++instSetCounters[M5checkpoint]; return M5checkpoint; }
	case 0x4F: {++instSetCounters[M5writefile]; return M5writefile; }
	case 0x50: {++instSetCounters[M5readfile]; return M5readfile; }
	case 0x51: {++instSetCounters[M5break]; return M5break; }
	case 0x52: {++instSetCounters[M5switchcpu]; return M5switchcpu; }
	case 0x53: {++instSetCounters[M5addsymbol]; return M5addsymbol; }
	case 0x54: {++instSetCounters[M5panic]; return M5panic; }
	case 0x5a: {++instSetCounters[M5workbegin]; return M5workbegin; }
	case 0x5b: {++instSetCounters[M5workend]; return M5workend; }
	}
	{++instSetCounters[Unknown32]; return Unknown32; }
}

inline Aarch32Set aarch32::LoadByteMemoryHints(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const uint8_t op1 = bits(machInst, 24, 23);
	const uint8_t op2 = bits(machInst, 11, 6);
	const IntRegIndex rn = (IntRegIndex)(uint8_t)bits(machInst, 19, 16);
	const IntRegIndex rt = (IntRegIndex)(uint8_t)bits(machInst, 15, 12);
	const IntRegIndex rm = (IntRegIndex)(uint8_t)bits(machInst, 3, 0);
	const uint8_t imm12 = bits(machInst, 11, 0);
	const uint8_t imm8 = bits(machInst, 7, 0);
	bool pldw = bits(machInst, 21) ? true : false;
	const uint8_t imm2 = bits(machInst, 5, 4);
	instType.isLoad = true;
	if (rn == 0xf) {
		if (rt == 0xf) {
			const bool add = bits(machInst, 23) ? true : false;
			if (bits(op1, 1) == 1) {
				if (add) {
					{++instSetCounters[Pli_iulit]; return Pli_iulit; }
				}
				else {
					{++instSetCounters[Pli_ilit]; return Pli_ilit; }
				}
			}
			else {
				if (add) {
					{++instSetCounters[Pld_iulit]; return Pld_iulit; }
				}
				else {
					{++instSetCounters[Pld_ilit]; return Pld_ilit; }
				}
			}
		}
		else {
			instReg.DReg[0] = rt;
			if (bits(op1, 1) == 1) {
				if (bits(machInst, 23)) {
					{++instSetCounters[Ldrsb_lit_u]; return Ldrsb_lit_u; }
				}
				else {
					{++instSetCounters[Ldrsb_lit]; return Ldrsb_lit; }
				}
			}
			else {
				if (bits(machInst, 23)) {
					{++instSetCounters[Ldrb_lit_u]; return Ldrb_lit_u; }
				}
				else {
					{++instSetCounters[Ldrb_lit]; return Ldrb_lit; }
				}
			}
		}
	}
	else if (rt == 0xf) {
		instReg.SReg[1] = rn;
		switch (op1) {
		case 0x0:
			if (op2 == 0x0) {
				instReg.SReg[0] = rm;
				if (pldw) {
					{++instSetCounters[Pldw_radd]; return Pldw_radd; }
				}
				else {
					{++instSetCounters[Pld_radd]; return Pld_radd; }
				}
			}
			else if (bits(op2, 5, 2) == 0xc) {
				if (pldw) {
					{++instSetCounters[Pldw_isub]; return Pldw_isub; }
				}
				else {
					{++instSetCounters[Pld_isub]; return Pld_isub; }
				}
			}
			break;
		case 0x1:
			if (pldw) {
				{++instSetCounters[Pldw_iadd]; return Pldw_iadd; }
			}
			else {
				{++instSetCounters[Pld_iadd]; return Pld_iadd; }
			}
		case 0x2:
			if (op2 == 0x0) {
				instReg.SReg[0] = rm;
				{++instSetCounters[Pli_radd]; return Pli_radd; }
			}
			else if (bits(op2, 5, 2) == 0xc) {
				{++instSetCounters[Pli_ilit]; return Pli_ilit; }
			}
			break;
		case 0x3:
		{++instSetCounters[Pli_iulit]; return Pli_iulit; }
		}
		{++instSetCounters[Unknown32]; return Unknown32; }
	}
	else {
		instReg.DReg[0] = rt;
		instReg.SReg[1] = rn;
		switch (op1) {
		case 0x0:
			if (op2 == 0) {
				instReg.SReg[0] = rm;
				{++instSetCounters[Ldrb_radd]; return Ldrb_radd; }
			}
			else if (bits(op2, 5, 2) == 0xe) {
				{++instSetCounters[Ldrbt]; return Ldrbt; }
			}
			else if ((op2 & 0x24) == 0x24 || bits(op2, 5, 2) == 0xc) {
				const uint8_t puw = bits(machInst, 10, 8);
				if (bits(puw, 0)) instReg.DReg[1] = rn;
				switch (puw) {
				case 0x1:
				{++instSetCounters[Ldrb_iw]; return Ldrb_iw; }
				case 0x3:
				{++instSetCounters[Ldrb_iuw]; return Ldrb_iuw; }
				case 0x4:
				{++instSetCounters[Ldrb_ip]; return Ldrb_ip; }
				case 0x5:
				{++instSetCounters[Ldrb_ipw]; return Ldrb_ipw; }
				case 0x7:
				{++instSetCounters[Ldrb_ipuw]; return Ldrb_ipuw; }
				}
			}
			break;
		case 0x1:
		{++instSetCounters[Ldrb_iadd]; return Ldrb_iadd; }
		case 0x2:
			if (op2 == 0) {
				instReg.SReg[0] = rm;
				{++instSetCounters[Ldrsb_radd]; return Ldrsb_radd; }
			}
			else if (bits(op2, 5, 2) == 0xe)
			{
				++instSetCounters[Ldrsbt]; return Ldrsbt;
			}
			else if ((op2 & 0x24) == 0x24 || bits(op2, 5, 2) == 0xc){
				const uint8_t puw = bits(machInst, 10, 8);
				if (bits(puw, 0)) instReg.DReg[1] = rn;
				switch (puw) {
				case 0x1:
				{++instSetCounters[Ldrsb_iw]; return Ldrsb_iw; }
				case 0x3:
				{++instSetCounters[Ldrsb_iuw]; return Ldrsb_iuw; }
				case 0x4:
				{++instSetCounters[Ldrsb_ip]; return Ldrsb_ip; }
				case 0x5:
				{++instSetCounters[Ldrsb_ipw]; return Ldrsb_ipw; }
				case 0x7:
				{++instSetCounters[Ldrsb_ipuw]; return Ldrsb_ipuw; }
				}
			}
			break;
		case 0x3:
		{++instSetCounters[Ldrsb_iadd]; return Ldrsb_iadd; }
		}
		{++instSetCounters[Unknown32]; return Unknown32; }
	}
}

inline Aarch32Set aarch32::LoadHalfwordMemoryHints(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const uint8_t op1 = bits(machInst, 24, 23);
	const uint8_t op2 = bits(machInst, 11, 6);
	const IntRegIndex rn = (IntRegIndex)(uint8_t)bits(machInst, 19, 16);
	const IntRegIndex rt = (IntRegIndex)(uint8_t)bits(machInst, 15, 12);
	const IntRegIndex rm = (IntRegIndex)(uint8_t)bits(machInst, 3, 0);
	const uint8_t imm12 = bits(machInst, 11, 0);
	const uint8_t imm8 = bits(machInst, 7, 0);
	bool pldw = bits(machInst, 21) ? true : false;
	const uint8_t imm2 = bits(machInst, 5, 4);
	if (rn == 0xf) {
		if (rt == 0xf) {
			if (bits(op1, 1) == 1) {
				// Unallocated memory hint
				{++instSetCounters[NopInst]; return NopInst; }
			}
			else {
				{++instSetCounters[Unknown32]; return Unknown32; }
			}
		}
		else {
			instReg.DReg[0] = rt;
			instType.isLoad = true;
			if (bits(op1, 1) == 1) {
				if (bits(machInst, 23)) {
					{++instSetCounters[Ldrsh_lit_u]; return Ldrsh_lit_u; }
				}
				else {
					{++instSetCounters[Ldrsh_lit]; return Ldrsh_lit; }
				}
			}
			else {
				if (bits(machInst, 23)) {
					{++instSetCounters[Ldrh_lit_u]; return Ldrh_lit_u; }
				}
				else {
					{++instSetCounters[Ldrh_lit]; return Ldrh_lit; }
				}
			}
		}
	}
	else if (rt == 0xf) {
		instReg.SReg[1] = rn;
		switch (op1) {
		case 0x0:
			if (op2 == 0x0) {
				instType.isLoad = true;
				if (pldw) {
					{++instSetCounters[Pldw_radd]; return Pldw_radd; }
				}
				else {
					{++instSetCounters[Pld_radd]; return Pld_radd; }
				}
			}
			else if (bits(op2, 5, 2) == 0xc) {
				instType.isLoad = true;
				if (pldw) {
					{++instSetCounters[Pldw_isub]; return Pldw_isub; }
				}
				else {
					{++instSetCounters[Pld_isub]; return Pld_isub; }
				}
			}
			break;
		case 0x1:
			if (pldw) {
				instType.isLoad = true;
				{++instSetCounters[Pldw_iadd]; return Pldw_iadd; }
			}
			else {
				instType.isLoad = true;
				{++instSetCounters[Pld_iadd]; return Pld_iadd; }
			}
		case 0x2:
			if (op2 == 0x0 || bits(op2, 5, 2) == 0xc) {
				// Unallocated memory hint
				{++instSetCounters[NopInst]; return NopInst; }
			}
			break;
		case 0x3:
		{++instSetCounters[NopInst]; return NopInst; }
		}
		{++instSetCounters[Unknown32]; return Unknown32; }
	}
	else {
		instReg.DReg[0] = rt;
		instReg.SReg[0] = rn;
		instType.isLoad = true;
		switch (op1) {
		case 0x0:
			if (op2 == 0) {
				instReg.SReg[1] = rm;
				{++instSetCounters[Ldrh_radd]; return Ldrh_radd; }
			}
			else if (bits(op2, 5, 2) == 0xe)
			{
				++instSetCounters[Ldrht]; return Ldrht;
			}
			else if ((op2 & 0x24) == 0x24 || bits(op2, 5, 2) == 0xc) {
				const uint8_t puw = bits(machInst, 10, 8);
				if (bits(puw, 0)) instReg.DReg[1] = rn;
				switch (puw) {
				case 0x1:
				{++instSetCounters[Ldrh_iw]; return Ldrh_iw; }
				case 0x3:
				{++instSetCounters[Ldrh_iuw]; return Ldrh_iuw; }
				case 0x4:
				{++instSetCounters[Ldrh_ip]; return Ldrh_ip; }
				case 0x5:
				{++instSetCounters[Ldrh_ipw]; return Ldrh_ipw; }
				case 0x7:
				{++instSetCounters[Ldrh_ipuw]; return Ldrh_ipuw; }
				}
			}
			break;
		case 0x1:
		{++instSetCounters[Ldrh_iadd]; return Ldrh_iadd; }
		case 0x2:
			if (op2 == 0){
				instReg.SReg[1] = rm;
				{++instSetCounters[Ldrsh_radd]; return Ldrsh_radd; }
			}
			else if (bits(op2, 5, 2) == 0xe)
			{
				++instSetCounters[Ldrsht]; return Ldrsht;
			}
			else if ((op2 & 0x24) == 0x24 || bits(op2, 5, 2) == 0xc) {
				const uint8_t puw = bits(machInst, 10, 8);
				if (bits(puw, 0)) instReg.DReg[1] = rn;
				switch (puw) {
				case 0x1:
				{++instSetCounters[Ldrsh_iw]; return Ldrsh_iw; }
				case 0x3:
				{++instSetCounters[Ldrsh_iuw]; return Ldrsh_iuw; }
				case 0x4:
				{++instSetCounters[Ldrsh_ip]; return Ldrsh_ip; }
				case 0x5:
				{++instSetCounters[Ldrsh_ipw]; return Ldrsh_ipw; }
				case 0x7:
				{++instSetCounters[Ldrsh_ipuw]; return Ldrsh_ipuw; }
				}
			}
			break;
		case 0x3:
		{++instSetCounters[Ldrsh_iadd]; return Ldrsh_iadd; }
		}
		{++instSetCounters[Unknown32]; return Unknown32; }
	}
}
// com
inline Aarch32Set aarch32::decodeVfpData(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const uint8_t opc1 = bits(machInst, 23, 20);
	const uint8_t opc2 = bits(machInst, 19, 16);
	const uint8_t opc3 = bits(machInst, 7, 6);
	//const uint8_t opc4 = bits(machInst, 3, 0);
	const bool single = (bits(machInst, 8) == 0);
	// Used to select between vcmp and vcmpe.
	const bool e = (bits(machInst, 7) == 1);
	IntRegIndex vd;
	IntRegIndex vm;
	IntRegIndex vn;
	if (single) {
		vd = (IntRegIndex)(bits(machInst, 22) | (bits(machInst, 15, 12) << 1));
		vm = (IntRegIndex)(bits(machInst, 5) | (bits(machInst, 3, 0) << 1));
		vn = (IntRegIndex)(bits(machInst, 7) | (bits(machInst, 19, 16) << 1));
	}
	else {
		vd = (IntRegIndex)((bits(machInst, 22) << 5) | (bits(machInst, 15, 12) << 1));
		vm = (IntRegIndex)((bits(machInst, 5) << 5) | (bits(machInst, 3, 0) << 1));
		vn = (IntRegIndex)((bits(machInst, 7) << 5) | (bits(machInst, 19, 16) << 1));
	}

	switch (opc1 & 0xb /* 1011 */) {
	case 0x0:{
		instReg.DReg[2] = vd;
		instReg.SReg[3] = vm;
		instReg.SReg[4] = vn;
		if (bits(machInst, 6) == 0) {
			if (single) {
				{++instSetCounters[VmlaS]; return VmlaS; }
			}
			else {
				{++instSetCounters[VmlaD]; return VmlaD; }
			}
		}
		else {
			if (single) {
				{++instSetCounters[VmlsS]; return VmlsS; }
			}
			else {
				{++instSetCounters[VmlsD]; return VmlsD; }
			}
		}
	}
	case 0x1:{
		instReg.DReg[2] = vd;
		instReg.SReg[3] = vm;
		instReg.SReg[4] = vn;
		if (bits(machInst, 6) == 1) {
			if (single) {
				{++instSetCounters[VnmlaS]; return VnmlaS; }
			}
			else {
				{++instSetCounters[VnmlaD]; return VnmlaD; }
			}
		}
		else {
			if (single) {
				{++instSetCounters[VnmlsS]; return VnmlsS; }
			}
			else {
				{++instSetCounters[VnmlsD]; return VnmlsD; }
			}
		}
	}
	case 0x2:{
		instReg.DReg[2] = vd;
		instReg.SReg[3] = vm;
		instReg.SReg[4] = vn;
		if ((opc3 & 0x1) == 0) {
			if (single) {
				{++instSetCounters[VmulS]; return VmulS; }
			}
			else {
				{++instSetCounters[VmulD]; return VmulD; }
			}
		}
		else {
			if (single) {
				{++instSetCounters[VnmulS]; return VnmulS; }
			}
			else {
				{++instSetCounters[VnmulD]; return VnmulD; }
			}
		}
	}
	case 0x3:{
		instReg.DReg[2] = vd;
		instReg.SReg[3] = vm;
		instReg.SReg[4] = vn;
		if ((opc3 & 0x1) == 0) {
			if (single) {
				{++instSetCounters[VaddS]; return VaddS; }
			}
			else {
				{++instSetCounters[VaddD]; return VaddD; }
			}
		}
		else {
			if (single) {
				{++instSetCounters[VsubS]; return VsubS; }
			}
			else {
				{++instSetCounters[VsubD]; return VsubD; }
			}
		}
	}
	case 0x8:{
		instReg.DReg[2] = vd;
		instReg.SReg[3] = vm;
		instReg.SReg[4] = vn;
		if ((opc3 & 0x1) == 0) {
			if (single) {
				{++instSetCounters[VdivS]; return VdivS; }
			}
			else {
				{++instSetCounters[VdivD]; return VdivD; }
			}
		}
		break;
	}
	case 0x9:{
		instReg.DReg[2] = vd;
		instReg.SReg[3] = vm;
		instReg.SReg[4] = vn;
		if ((opc3 & 0x1) == 0) {
			if (single) {
				{++instSetCounters[VfnmaS]; return VfnmaS; }
			}
			else {
				{++instSetCounters[VfnmaD]; return VfnmaD; }
			}
		}
		else {
			if (single) {
				{++instSetCounters[VfnmsS]; return VfnmsS; }
			}
			else {
				{++instSetCounters[VfnmsD]; return VfnmsD; }
			}
		}
		break;
	}
	case 0xa:{
		instReg.DReg[2] = vd;
		instReg.SReg[3] = vm;
		instReg.SReg[4] = vn;
		if ((opc3 & 0x1) == 0) {
			if (single) {
				{++instSetCounters[VfmaS]; return VfmaS; }
			}
			else {
				{++instSetCounters[VfmaD]; return VfmaD; }
			}
		}
		else {
			if (single) {
				{++instSetCounters[VfmsS]; return VfmsS; }
			}
			else {
				{++instSetCounters[VfmsD]; return VfmsD; }
			}
		}
		break;
	}
	case 0xb:{
		instReg.DReg[2] = vd;
		if ((opc3 & 0x1) == 0) {
			const uint8_t baseImm = bits(machInst, 3, 0) | (bits(machInst, 19, 16) << 4);
			if (single) {
				uint8_t imm = 0;//vfp_modified_imm(baseImm, false);
				{++instSetCounters[VmovImmS]; return VmovImmS; }
			}
			else {
				uint64_t imm = 0;//vfp_modified_imm(baseImm, true);
				{++instSetCounters[VmovImmD]; return VmovImmD; }
			}
		}
		instReg.SReg[3] = vm;
		switch (opc2) {
		case 0x0:
			if (opc3 == 1) {
				if (single) {
					{++instSetCounters[VmovRegS]; return VmovRegS; }
				}
				else {
					{++instSetCounters[VmovRegD]; return VmovRegD; }
				}
			}
			else {
				if (single) {
					{++instSetCounters[VabsS]; return VabsS; }
				}
				else {
					{++instSetCounters[VabsD]; return VabsD; }
				}
			}
		case 0x1:
			if (opc3 == 1) {
				if (single) {
					{++instSetCounters[VnegS]; return VnegS; }
				}
				else {
					{++instSetCounters[VnegD]; return VnegD; }
				}
			}
			else {
				if (single) {
					{++instSetCounters[VsqrtS]; return VsqrtS; }
				}
				else {
					{++instSetCounters[VsqrtD]; return VsqrtD; }
				}
			}
		case 0x2:
		case 0x3:{
			instReg.DReg[2] = vd;
			instReg.SReg[3] = vm;
			const bool toHalf = bits(machInst, 16) ? true : false;
			const bool top = bits(machInst, 7) ? true : false;
			if (top) {
				if (toHalf)
				{
					++instSetCounters[VcvtFpSFpH]; return VcvtFpSFpH;
				}
				else
				{
					++instSetCounters[VcvtFpHTFpS]; return VcvtFpHTFpS;
				}
			}
			else {
				if (toHalf)
				{
					++instSetCounters[VcvtFpSFpHB]; return VcvtFpSFpHB;
				}
				else
				{
					++instSetCounters[VcvtFpHBFpS]; return VcvtFpHBFpS;
				}
			}
		}
		case 0x4:{
			instReg.DReg[2] = vd;
			instReg.SReg[3] = vm;
			instReg.SReg[4] = vn;
			if (single) {
				if (e)
				{
					++instSetCounters[VcmpeS]; return VcmpeS;
				}
				else
				{
					++instSetCounters[VcmpS]; return VcmpS;
				}
			}
			else {
				if (e)
				{
					++instSetCounters[VcmpeD]; return VcmpeD;
				}
				else
				{
					++instSetCounters[VcmpD]; return VcmpD;
				}
			}
		}
		case 0x5:{
			instReg.DReg[2] = vd;
			if (single) {
				if (e) {
					{++instSetCounters[VcmpeZeroS]; return VcmpeZeroS; }
				}
				else {
					{++instSetCounters[VcmpZeroS]; return VcmpZeroS; }
				}
			}
			else {
				if (e) {
					{++instSetCounters[VcmpeZeroD]; return VcmpeZeroD; }
				}
				else {
					{++instSetCounters[VcmpZeroD]; return VcmpZeroD; }
				}
			}
		}
		case 0x7:
			if (opc3 == 0x3) {
				instReg.SReg[3] = vm;
				if (single) {
					vd = (IntRegIndex)((bits(machInst, 22) << 5) | (bits(machInst, 15, 12) << 1));
					instReg.DReg[2] = vd;
					{++instSetCounters[VcvtFpSFpD]; return VcvtFpSFpD; }
				}
				else {
					vd = (IntRegIndex)(bits(machInst, 22) | (bits(machInst, 15, 12) << 1));
					instReg.DReg[2] = vd;
					{++instSetCounters[VcvtFpDFpS]; return VcvtFpDFpS; }
				}
			}
			break;
		case 0x8:{
			instReg.DReg[2] = vd;
			if (bits(machInst, 7) == 0) {
				if (single) {
					instReg.SReg[3] = vm;
					{++instSetCounters[VcvtUIntFpS]; return VcvtUIntFpS; }
				}
				else {
					vm = (IntRegIndex)(bits(machInst, 5) | (bits(machInst, 3, 0) << 1));
					instReg.SReg[3] = vm;
					{++instSetCounters[VcvtUIntFpD]; return VcvtUIntFpD; }
				}
			}
			else {
				if (single) {
					instReg.SReg[3] = vm;
					{++instSetCounters[VcvtSIntFpS]; return VcvtSIntFpS; }
				}
				else {
					vm = (IntRegIndex)(bits(machInst, 5) | (bits(machInst, 3, 0) << 1));
					instReg.SReg[3] = vm;
					{++instSetCounters[VcvtSIntFpD]; return VcvtSIntFpD; }
				}
			}
		}
		case 0xa:{
			const bool half = (bits(machInst, 7) == 0);
			const uint8_t imm = bits(machInst, 5) | (bits(machInst, 3, 0) << 1);
			const uint8_t size = (bits(machInst, 7) == 0 ? 16 : 32) - imm;
			instReg.DReg[2] = vd;
			instReg.SReg[3] = vd;
			if (single) {
				if (half) {
					{++instSetCounters[VcvtSHFixedFpS]; return VcvtSHFixedFpS; }
				}
				else {
					{++instSetCounters[VcvtSFixedFpS]; return VcvtSFixedFpS; }
				}
			}
			else {
				if (half) {
					{++instSetCounters[VcvtSHFixedFpD]; return VcvtSHFixedFpD; }
				}
				else {
					{++instSetCounters[VcvtSFixedFpD]; return VcvtSFixedFpD; }
				}
			}
		}
		case 0xb:{
			const bool half = (bits(machInst, 7) == 0);
			const uint8_t imm = bits(machInst, 5) | (bits(machInst, 3, 0) << 1);
			const uint8_t size = (bits(machInst, 7) == 0 ? 16 : 32) - imm;
			instReg.DReg[2] = vd;
			instReg.SReg[3] = vd;
			if (single) {
				if (half)
				{
					++instSetCounters[VcvtUHFixedFpS]; return VcvtUHFixedFpS;
				}
				else
				{
					++instSetCounters[VcvtUFixedFpS]; return VcvtUFixedFpS;
				}
			}
			else {
				if (half)
				{
					++instSetCounters[VcvtUHFixedFpD]; return VcvtUHFixedFpD;
				}
				else
				{
					++instSetCounters[VcvtUFixedFpD]; return VcvtUFixedFpD;
				}
			}
		}
		case 0xc:{
			instReg.SReg[3] = vm;
			if (bits(machInst, 7) == 0) {
				if (single) {
					instReg.DReg[2] = vd;
					{++instSetCounters[VcvtFpUIntSR]; return VcvtFpUIntSR; }
				}
				else {
					vd = (IntRegIndex)(bits(machInst, 22) | (bits(machInst, 15, 12) << 1));
					instReg.DReg[2] = vd;
					{++instSetCounters[VcvtFpUIntDR]; return VcvtFpUIntDR; }
				}
			}
			else {
				if (single) {
					instReg.DReg[2] = vd;
					{++instSetCounters[VcvtFpUIntS]; return VcvtFpUIntS; }
				}
				else {
					vd = (IntRegIndex)(bits(machInst, 22) | (bits(machInst, 15, 12) << 1));
					instReg.DReg[2] = vd;
					{++instSetCounters[VcvtFpUIntD]; return VcvtFpUIntD; }
				}
			}
		}
		case 0xd:{
			instReg.SReg[3] = vm;
			if (bits(machInst, 7) == 0) {
				if (single) {
					instReg.DReg[2] = vd;
					{++instSetCounters[VcvtFpSIntSR]; return VcvtFpSIntSR; }
				}
				else {
					vd = (IntRegIndex)(bits(machInst, 22) | (bits(machInst, 15, 12) << 1));
					instReg.DReg[2] = vd;
					{++instSetCounters[VcvtFpSIntDR]; return VcvtFpSIntDR; }
				}
			}
			else {
				if (single) {
					instReg.DReg[2] = vd;
					{++instSetCounters[VcvtFpSIntS]; return VcvtFpSIntS; }
				}
				else {
					vd = (IntRegIndex)(bits(machInst, 22) | (bits(machInst, 15, 12) << 1));
					instReg.DReg[2] = vd;
					{++instSetCounters[VcvtFpSIntD]; return VcvtFpSIntD; }
				}
			}
		}
		case 0xe:{
			const bool half = (bits(machInst, 7) == 0);
			const uint8_t imm = bits(machInst, 5) | (bits(machInst, 3, 0) << 1);
			const uint8_t size = (bits(machInst, 7) == 0 ? 16 : 32) - imm;
			instReg.DReg[2] = vd;
			instReg.SReg[3] = vd;
			if (single) {
				if (half)
				{
					++instSetCounters[VcvtFpSHFixedS]; return VcvtFpSHFixedS;
				}
				else
				{
					++instSetCounters[VcvtFpSFixedS]; return VcvtFpSFixedS;
				}
			}
			else {
				if (half)
				{
					++instSetCounters[VcvtFpSHFixedD]; return VcvtFpSHFixedD;
				}
				else
				{
					++instSetCounters[VcvtFpSFixedD]; return VcvtFpSFixedD;
				}
			}
		}
		case 0xf:{
			const bool half = (bits(machInst, 7) == 0);
			const uint8_t imm = bits(machInst, 5) | (bits(machInst, 3, 0) << 1);
			const uint8_t size = (bits(machInst, 7) == 0 ? 16 : 32) - imm;
			instReg.DReg[2] = vd;
			instReg.SReg[3] = vd;
			if (single) {
				if (half)
				{
					++instSetCounters[VcvtFpUHFixedS]; return VcvtFpUHFixedS;
				}
				else
				{
					++instSetCounters[VcvtFpUFixedS]; return VcvtFpUFixedS;
				}
			}
			else {
				if (half)
				{
					++instSetCounters[VcvtFpUHFixedD]; return VcvtFpUHFixedD;
				}
				else
				{
					++instSetCounters[VcvtFpUFixedD]; return VcvtFpUFixedD;
				}
			}
		}
		}
		break;
	}
	++instSetCounters[Unknown32]; 
	return Unknown32;
	}
	{++instSetCounters[Unknown32]; return Unknown32; }
}
// recode by shen 11/9
inline Aarch32Set aarch32::decodeShortFpTransfer(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const uint8_t l = bits(machInst, 20);
	const uint8_t c = bits(machInst, 8);
	const uint8_t a = bits(machInst, 23, 21);
	const uint8_t b = bits(machInst, 6, 5);
	if ((THUMB == 1 && bits(machInst, 28) == 1) ||
		(THUMB == 0 && COND_CODE == 0xf)) {
		// Determine if this is backported aarch64 FP instruction
		const bool b31_b24 = bits(machInst, 31, 24) == 0xFE;
		const bool b23 = bits(machInst, 23);
		const bool b21_b18 = bits(machInst, 21, 18) == 0xE;
		const bool b11_b9 = bits(machInst, 11, 9) == 0x5;
		const bool sz = bits(machInst, 8);
		const bool b7_b6 = bits(machInst, 7, 6) == 0x1;
		const bool b6 = bits(machInst, 6) == 0x0;
		const bool b4 = bits(machInst, 4) == 0x0;
		if (b31_b24 && b23 && b21_b18 && b11_b9 && b7_b6 && b4) {
			// VINT* Integer Rounding Instructon
			const uint32_t rm = bits(machInst, 17, 16);
			if (sz) {
				const IntRegIndex vd =
					(IntRegIndex)((bits(machInst, 22) << 5) |
					(bits(machInst, 15, 12) << 1));
				const IntRegIndex vm =
					(IntRegIndex)((bits(machInst, 5) << 5) |
					(bits(machInst, 3, 0) << 1));
				instReg.DReg[2] = vd;
				instReg.SReg[3] = vm;
				switch (rm) {
				case 0x0:
					++instSetCounters[VRIntAD]; return VRIntAD;
				case 0x1:
					++instSetCounters[VRIntND]; return VRIntND;
				case 0x2:
					++instSetCounters[VRIntPD]; return VRIntPD;
				case 0x3:
					++instSetCounters[VRIntMD]; return VRIntMD;
				default: ++instSetCounters[Unknown32]; return Unknown32;
				}
			}
			else {
				const IntRegIndex vd =
					(IntRegIndex)(bits(machInst, 22) |
					(bits(machInst, 15, 12) << 1));
				const IntRegIndex vm =
					(IntRegIndex)(bits(machInst, 5) |
					(bits(machInst, 3, 0) << 1));
				instReg.DReg[2] = vd;
				instReg.SReg[3] = vm;
				switch (rm) {
				case 0x0:
					++instSetCounters[VRIntAS]; return VRIntAS;
				case 0x1:
					++instSetCounters[VRIntNS]; return VRIntNS;
				case 0x2:
					++instSetCounters[VRIntPS]; return VRIntPS;
				case 0x3:
					++instSetCounters[VRIntMS]; return VRIntMS;
				default: ++instSetCounters[Unknown32]; return Unknown32;
				}
			}
		}
		else if (b31_b24 && !b23 && b11_b9 && b6 && b4){
			// VSEL* floating point conditional select
			ConditionCode cond;
			switch (bits(machInst, 21, 20)) {
			case 0x0: cond = COND_EQ; break;
			case 0x1: cond = COND_VS; break;
			case 0x2: cond = COND_GE; break;
			case 0x3: cond = COND_GT; break;
			}
			if (sz) {
				const IntRegIndex vd =
					(IntRegIndex)((bits(machInst, 22) << 5) |
					(bits(machInst, 15, 12) << 1));
				const IntRegIndex vm =
					(IntRegIndex)((bits(machInst, 5) << 5) |
					(bits(machInst, 3, 0) << 1));
				const IntRegIndex vn =
					(IntRegIndex)((bits(machInst, 7) << 5) |
					(bits(machInst, 19, 16) << 1));
				instReg.DReg[2] = vd;
				instReg.SReg[3] = vn;
				instReg.SReg[4] = vm;
				++instSetCounters[VselD]; return VselD;
			}
			else {
				const IntRegIndex vd =
					(IntRegIndex)(bits(machInst, 22) |
					(bits(machInst, 15, 12) << 1));
				const IntRegIndex vm =
					(IntRegIndex)(bits(machInst, 5) |
					(bits(machInst, 3, 0) << 1));
				const IntRegIndex vn =
					(IntRegIndex)((bits(machInst, 19, 16) << 1) |
					bits(machInst, 7));
				instReg.DReg[2] = vd;
				instReg.SReg[3] = vn;
				instReg.SReg[4] = vm;
				++instSetCounters[VselS]; return VselS;
			}
		}
		else {
			++instSetCounters[Unknown32]; return Unknown32;
		}
	}

	if (l == 0 && c == 0) {
		if (a == 0) {
			const IntRegIndex vn = (IntRegIndex)(uint8_t)((bits(machInst, 19, 16) << 1) | bits(machInst, 7));
			const IntRegIndex rt = (IntRegIndex)(uint8_t)bits(machInst, 15, 12);
			if (bits(machInst, 20) == 1) {
				instReg.DReg[0] = rt;
				instReg.SReg[3] = vn;
				{++instSetCounters[VmovRegCoreW]; return VmovRegCoreW; }
			}
			else {
				instReg.DReg[2] = vn;
				instReg.SReg[0] = rt;
				{++instSetCounters[VmovCoreRegW]; return VmovCoreRegW; }
			}
		}
		else if (a == 0x7) {
			const IntRegIndex rt = (IntRegIndex)(uint8_t)bits(machInst, 15, 12);
			instReg.SReg[0] = rt;
			uint8_t reg = bits(machInst, 19, 16);
			uint8_t specReg;
			switch (reg) {
			case 0:
				specReg = 10;
				break;
			case 1:
				specReg = 11;
				break;
			case 6:
				specReg = 12;
				break;
			case 7:
				specReg = 13;
				break;
			case 8:
				specReg = 14;
				break;
			default:
			{++instSetCounters[Unknown32]; return Unknown32; }
			}
			if (specReg == 11) {
				{++instSetCounters[VmsrFpscr]; return VmsrFpscr; }
			}
			else {
				{++instSetCounters[Vmsr]; return Vmsr; }
			}
		}
	}
	else if (l == 0 && c == 1) {
		if (bits(a, 2) == 0) {
			uint8_t vd = (bits(machInst, 7) << 5) | (bits(machInst, 19, 16) << 1);
			// Handle accessing each single precision half of the vector.
			vd += bits(machInst, 21);
			const IntRegIndex rt = (IntRegIndex)(uint8_t)bits(machInst, 15, 12);
			instReg.DReg[2] = vd;
			instReg.SReg[0] = rt;
			if (bits(machInst, 22) == 1)
			{
				++instSetCounters[VmovCoreRegB]; return VmovCoreRegB;
			}
			else if (bits(machInst, 5) == 1)
			{
				++instSetCounters[VmovCoreRegH]; return VmovCoreRegH;
			}
			else if (bits(machInst, 6) == 0)
			{
				++instSetCounters[VmovCoreRegW]; return VmovCoreRegW;
			}
			else
			{
				++instSetCounters[Unknown32]; return Unknown32;
			}
		}
		else if (bits(b, 1) == 0) {
			bool q = bits(machInst, 21) ? true : false;
			unsigned be = (bits(machInst, 22) << 1) | (bits(machInst, 5));
			IntRegIndex vd = (IntRegIndex)(2 * (uint8_t)(bits(machInst, 19, 16) | (bits(machInst, 7) << 4)));
			IntRegIndex rt = (IntRegIndex)(uint8_t)bits(machInst, 15, 12);
			instReg.DReg[2] = vd;
			instReg.SReg[0] = rt;
			if (q) {
				switch (be) {
				case 0:
				{++instSetCounters[NVdupQGpr]; return NVdupQGpr; }
				case 1:
				{++instSetCounters[NVdupQGpr]; return NVdupQGpr; }
				case 2:
				{++instSetCounters[NVdupQGpr]; return NVdupQGpr; }
				case 3:
				{++instSetCounters[Unknown32]; return Unknown32; }
				}
			}
			else {
				switch (be) {
				case 0:
				{++instSetCounters[NVdupDGpr]; return NVdupDGpr; }
				case 1:
				{++instSetCounters[NVdupDGpr]; return NVdupDGpr; }
				case 2:
				{++instSetCounters[NVdupDGpr]; return NVdupDGpr; }
				case 3:
				{++instSetCounters[Unknown32]; return Unknown32; }
				}
			}
		}
	}
	else if (l == 1 && c == 0) {
		if (a == 0) {
			const IntRegIndex vn = (IntRegIndex)(uint8_t)((bits(machInst, 19, 16) << 1) | bits(machInst, 7));
			const IntRegIndex rt = (IntRegIndex)(uint8_t)bits(machInst, 15, 12);
			instReg.SReg[0] = rt;
			if (bits(machInst, 20) == 1) {
				instReg.DReg[0] = rt;
				instReg.SReg[3] = vn;
				{++instSetCounters[VmovRegCoreW]; return VmovRegCoreW; }
			}
			else {
				instReg.DReg[2] = vn;
				instReg.SReg[0] = rt;
				{++instSetCounters[VmovCoreRegW]; return VmovCoreRegW; }
			}
		}
		else if (a == 7) {
			const IntRegIndex rt = (IntRegIndex)(uint8_t)bits(machInst, 15, 12);
			uint8_t reg = bits(machInst, 19, 16);
			uint8_t specReg;
			switch (reg) {
			case 0:
				specReg = MISCREG_FPSID;
				break;
			case 1:
				specReg = MISCREG_FPSCR;
				break;
			case 6:
				specReg = MISCREG_MVFR1;
				break;
			case 7:
				specReg = MISCREG_MVFR0;
				break;
			case 8:
				specReg = MISCREG_FPEXC;
				break;
			default:
			{++instSetCounters[Unknown32]; return Unknown32; }
			}
			if (rt == 0xf) {
				if (specReg == 11) {
					{++instSetCounters[VmrsApsrFpscr]; return VmrsApsrFpscr; }
				}
				else {
					{++instSetCounters[Unknown32]; return Unknown32; }
				}
			}
			else if (specReg == 11) {
				instReg.SReg[0] = rt;
				{++instSetCounters[VmrsFpscr]; return VmrsFpscr; }
			}
			else {
				instReg.SReg[0] = rt;
				{++instSetCounters[Vmrs]; return Vmrs; }
			}
		}
	}
	else {
		uint8_t vd = (bits(machInst, 7) << 5) | (bits(machInst, 19, 16) << 1);
		// Handle indexing into each single precision half of the vector.
		vd += bits(machInst, 21);
		uint8_t index;
		const IntRegIndex rt = (IntRegIndex)(uint8_t)bits(machInst, 15, 12);
		const bool u = (bits(machInst, 23) == 1);
		instReg.DReg[2] = vd;
		instReg.SReg[0] = rt;
		if (bits(machInst, 22) == 1) {
			index = bits(machInst, 6, 5);
			if (u) {
				{++instSetCounters[VmovRegCoreUB]; return VmovRegCoreUB; }
			}
			else {
				{++instSetCounters[VmovRegCoreSB]; return VmovRegCoreSB; }
			}
		}
		else if (bits(machInst, 5) == 1) {
			index = bits(machInst, 6);
			if (u) {
				{++instSetCounters[VmovRegCoreUH]; return VmovRegCoreUH; }
			}
			else {
				{++instSetCounters[VmovRegCoreSH]; return VmovRegCoreSH; }
			}
		}
		else if (bits(machInst, 6) == 0 && !u) {
			{++instSetCounters[VmovRegCoreW]; return VmovRegCoreW; }
		}
		else {
			{++instSetCounters[Unknown32]; return Unknown32; }
		}
	}
	++instSetCounters[Unknown32];
	return Unknown32;
}
// com 
inline Aarch32Set aarch32::decodeMcrMrc14(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const uint8_t opc1 = bits(machInst, 23, 21);
	const uint8_t crn = bits(machInst, 19, 16);
	const uint8_t opc2 = bits(machInst, 7, 5);
	const uint8_t crm = bits(machInst, 3, 0);
	const MiscRegIndex miscReg = decodeCP14Reg(crn, opc1, crm, opc2);
	const IntRegIndex rt = (IntRegIndex)(uint8_t)bits(machInst, 15, 12);
	const bool isRead = bits(machInst, 20) ? true : false;
	switch (miscReg) {
	case MISCREG_NOP:
	{++instSetCounters[NopInst]; return NopInst; }
	case MISCREG_CP14_UNIMPL:
	{++instSetCounters[FailUnimplemented]; return FailUnimplemented; }
	default:
		if (isRead) {
			instReg.DReg[0] = rt;
			{++instSetCounters[Mrc14]; return Mrc14; }
		}
		else {
			instReg.SReg[0] = rt;
			{++instSetCounters[Mcr14]; return Mcr14; }
		}
	}
}
// com 
inline Aarch32Set aarch32::decodeMcrMrc15(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const uint8_t opc1 = bits(machInst, 23, 21);
	const uint8_t crn = bits(machInst, 19, 16);
	const uint8_t opc2 = bits(machInst, 7, 5);
	const uint8_t crm = bits(machInst, 3, 0);
	const MiscRegIndex miscReg = decodeCP15Reg(crn, opc1, crm, opc2);
	const IntRegIndex rt = (IntRegIndex)(uint8_t)bits(machInst, 15, 12);
	const bool isRead = bits(machInst, 20) ? true : false;

	switch (miscReg) {
	case MISCREG_NOP:
	{++instSetCounters[NopInst]; return NopInst; }
	case MISCREG_CP15_UNIMPL:
	{++instSetCounters[FailUnimplemented]; return FailUnimplemented; }
	case MISCREG_DCCMVAC:
	{++instSetCounters[FlushPipeInst]; return FlushPipeInst; }
	case MISCREG_CP15ISB:
	{instType.isSerial = true; ++instSetCounters[Isb]; return Isb; }
	case MISCREG_CP15DSB:
	{instType.isSerial = true; ++instSetCounters[Dsb]; return Dsb; }
	case MISCREG_CP15DMB:
	{instType.isSerial = true; ++instSetCounters[Dmb]; return Dmb; }
	default:
		//return FailUnimplemented; //added by WenjFu
		// deleted by WenjFu
		if (miscRegInfo[miscReg][MISCREG_WARN_NOT_FAIL]) {
			//std::string full_mnem = csprintf("%s %s",
			//    isRead ? "mrc" : "mcr", miscRegName[miscReg]);
			//warn("\\tinstruction '%s' unimplemented\\n", full_mnem);
			// delete end

			// Remove the warn flag and set the implemented flag. This
			// prevents the instruction warning a second time, it also
			// means the instruction is actually generated. Actually
			// creating the instruction to access an register that isn't
			// implemented sounds a bit silly, but its required to get
			// the correct behaviour for hyp traps and undef exceptions.
			miscRegInfo[miscReg][MISCREG_IMPLEMENTED] = true;
			miscRegInfo[miscReg][MISCREG_WARN_NOT_FAIL] = false;
		}

		if (miscRegInfo[miscReg][MISCREG_IMPLEMENTED]) {
			if (isRead){
				instReg.DReg[0] = rt;
				{++instSetCounters[Mrc15]; return Mrc15; }
			}
			instReg.SReg[0] = rt;
			{++instSetCounters[Mcr15]; return Mcr15; }
		}
		else {
			{++instSetCounters[FailUnimplemented]; return FailUnimplemented; }
		}
	}
}
// com // recode by shen 11/9
inline Aarch32Set aarch32::decodeExtensionRegLoadStore(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const uint8_t opcode = bits(machInst, 24, 20);
	const uint8_t offset = bits(machInst, 7, 0);
	const bool single = (bits(machInst, 8) == 0);
	const IntRegIndex rn = (IntRegIndex)(uint8_t)bits(machInst, 19, 16);
	uint16_t vd;
	if (single)
		vd = (uint16_t)(uint8_t)((bits(machInst, 15, 12) << 1) | bits(machInst, 22));
	else
		vd = (uint16_t)(uint8_t)((bits(machInst, 15, 12) << 1) | (bits(machInst, 22) << 5));
	switch (bits(opcode, 4, 3)){
	case 0x0:{
		if (bits(opcode, 4, 1) == 0x2 && !(THUMB == 1 && bits(machInst, 28) == 1) &&
			!(THUMB == 0 && COND_CODE == 0xf)) {
			if ((bits(machInst, 7, 4) & 0xd) != 1) break;
			const IntRegIndex rt = (IntRegIndex)(uint8_t)bits(machInst, 15, 12);
			const IntRegIndex rt2 = (IntRegIndex)(uint8_t)bits(machInst, 19, 16);
			const bool op = bits(machInst, 20) ? true : false;
			uint8_t vm;
			if (single)
				vm = (bits(machInst, 3, 0) << 1) | bits(machInst, 5);
			else
				vm = (bits(machInst, 3, 0) << 1) | (bits(machInst, 5) << 5);
			if (op){
				instReg.SReg[3] = vm;
				instReg.DReg[0] = rt;
				instReg.DReg[1] = rt2;
				{++instSetCounters[Vmov2Core2Reg]; return Vmov2Core2Reg; }
			}
			else{
				instReg.DReg[2] = vm;
				instReg.SReg[0] = rt;
				instReg.SReg[1] = rt2;
				{++instSetCounters[Vmov2Reg2Core]; return Vmov2Reg2Core; }
			}
		}
		break;
	}
	case 0x1:{
		const int NumFloatV7ArchRegs = 64;
		const int NumFloatV8ArchRegs = 128;
		const int NumFloatSpecialRegs = 32;
		if (offset == 0 || vd + offset / 2 > NumFloatV7ArchRegs) break;
		if (bits(opcode, 1)) instReg.DReg[0] = rn;
		switch (bits(opcode, 1, 0)) {
		case 0x0:{
			instReg.SReg[3] = vd;
			instReg.SReg[0] = rn;
			{instType.isStore = true; ++instSetCounters[VLdmStmSt]; return VLdmStmSt; }
		}
		case 0x1:{
			instReg.DReg[2] = vd;
			instReg.SReg[0] = rn;
			{instType.isLoad = true; ++instSetCounters[VLdmStmLd]; return VLdmStmLd; }
		}
		case 0x2:{
			instReg.SReg[3] = vd;
			instReg.SReg[0] = rn;
			{instType.isStore = true; ++instSetCounters[VLdmStmSt]; return VLdmStmSt; }
		}
		case 0x3:{
			instReg.DReg[2] = vd;
			instReg.SReg[0] = rn;
			// If rn == sp, then this is called vpop.
			{instType.isLoad = true; ++instSetCounters[VLdmStmLd]; return VLdmStmLd; }
		}
		}
	}
	case 0x2:{
		instReg.DReg[0] = rn;
		if (bits(opcode, 1, 0) == 0x2) {
			instReg.SReg[3] = vd;
			instReg.SReg[0] = rn;
			// If rn == sp, then this is called vpush.
			{instType.isStore = true; ++instSetCounters[VLdmStmSt]; return VLdmStmSt; }
		}
		else if (bits(opcode, 1, 0) == 0x3){
			instReg.DReg[2] = vd;
			instReg.SReg[0] = rn;
			{instType.isLoad = true; ++instSetCounters[VLdmStmLd]; return VLdmStmLd; }
		}
	}
			 // Fall through on purpose
	case 0x3:{
		const bool up = (bits(machInst, 23) == 1);
		const uint8_t imm = bits(machInst, 7, 0) << 2;
		if (single)
			vd = (uint16_t)(uint8_t)((bits(machInst, 15, 12) << 1) | (bits(machInst, 22)));
		else
			vd = (uint16_t)(uint8_t)((bits(machInst, 15, 12) << 1) | (bits(machInst, 22) << 5));
		instReg.SReg[0] = rn;
		if (bits(opcode, 1, 0) == 0x0) {
			instReg.SReg[3] = vd;
			instType.isStore = true;
			if (single) {
				if (up)
				{
					++instSetCounters[Vstr_us]; return Vstr_us;
				}
				else
				{
					++instSetCounters[Vstr_s]; return Vstr_s;
				}
			}
			else {
				if (up)
				{
					++instSetCounters[Vstr_ud]; return Vstr_ud;
				}
				else
				{
					++instSetCounters[Vstr_d]; return Vstr_d;
				}
			}
		}
		else if (bits(opcode, 1, 0) == 0x1) {
			instReg.SReg[3] = vd;
			instType.isLoad = true;
			if (single) {
				if (up)
				{
					++instSetCounters[Vldr_us]; return Vldr_us;
				}
				else
				{
					++instSetCounters[Vldr_s]; return Vldr_s;
				}
			}
			else {
				if (up)
				{
					++instSetCounters[Vldr_ud]; return Vldr_ud;
				}
				else
				{
					++instSetCounters[Vldr_d]; return Vldr_d;
				}
			}
		}
	}
	default:
	{++instSetCounters[Unknown32]; return Unknown32; }
	}
	{++instSetCounters[Unknown32]; return Unknown32; }
}

inline Aarch32Set aarch32::decodeNeonData(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const bool u = bits(machInst, 28) ? true : false;
	const uint8_t a = bits(machInst, 23, 19);
	const uint8_t b = bits(machInst, 11, 8);
	const uint8_t c = bits(machInst, 7, 4);
	if (bits(a, 4) == 0) {
		return decodeNeonThreeRegistersSameLength(machInst, instSetCounters, instReg, instType);
	}
	else if ((c & 0x9) == 1) {
		if ((a & 0x7) == 0) {
			return decodeNeonOneRegModImm(machInst, instSetCounters, instReg, instType);
		}
		else {
			return decodeNeonTwoRegAndShift(machInst, instSetCounters, instReg, instType);
		}
	}
	else if ((c & 0x9) == 9) {
		return decodeNeonTwoRegAndShift(machInst, instSetCounters, instReg, instType);
	}
	else if (bits(a, 2, 1) != 0x3) {
		if ((c & 0x5) == 0) {
			return decodeNeonThreeRegDiffLengths(machInst, instSetCounters, instReg, instType);
		}
		else if ((c & 0x5) == 4) {
			return decodeNeonTwoRegScalar(machInst, instSetCounters, instReg, instType);
		}
	}
	else if ((a & 0x16) == 0x16) {
		const IntRegIndex vd = (IntRegIndex)(2 * (bits(machInst, 15, 12) | (bits(machInst, 22) << 4)));
		const IntRegIndex vn = (IntRegIndex)(2 * (bits(machInst, 19, 16) | (bits(machInst, 7) << 4)));
		const IntRegIndex vm = (IntRegIndex)(2 * (bits(machInst, 3, 0) | (bits(machInst, 5) << 4)));
		if (!u) {
			instReg.DReg[2] = vd;
			instReg.SReg[3] = vm;
			instReg.SReg[4] = vn;
			if (bits(c, 0) == 0) {
				unsigned imm4 = bits(machInst, 11, 8);
				bool q = bits(machInst, 6) ? true : false;
				if (imm4 >= 16 && !q)
				{
					++instSetCounters[Unknown32]; return Unknown32;
				}
				if (q) {
					{++instSetCounters[NVextQ]; return NVextQ; }
				}
				else {
					{++instSetCounters[NVextD]; return NVextD; }
				}
			}
		}
		else if (bits(b, 3) == 0 && bits(c, 0) == 0) {
			return decodeNeonTwoRegMisc(machInst, instSetCounters, instReg, instType);
		}
		else if (bits(b, 3, 2) == 0x2 && bits(c, 0) == 0) {
			unsigned length = bits(machInst, 9, 8) + 1;
			if ((uint8_t)vn / 2 + length > 32)
			{
				++instSetCounters[Unknown32]; return Unknown32;
			}
			instReg.DReg[2] = vd;
			instReg.SReg[3] = vm;
			instReg.SReg[4] = vn;
			if (bits(machInst, 6) == 0) {
				switch (length) {
				case 1:
				{++instSetCounters[NVtbl1]; return NVtbl1; }
				case 2:
				{++instSetCounters[NVtbl2]; return NVtbl2; }
				case 3:
				{++instSetCounters[NVtbl3]; return NVtbl3; }
				case 4:
				{++instSetCounters[NVtbl4]; return NVtbl4; }
				}
			}
			else {
				switch (length) {
				case 1:
				{++instSetCounters[NVtbx1]; return NVtbx1; }
				case 2:
				{++instSetCounters[NVtbx2]; return NVtbx2; }
				case 3:
				{++instSetCounters[NVtbx3]; return NVtbx3; }
				case 4:
				{++instSetCounters[NVtbx4]; return NVtbx4; }
				}
			}
		}
		else if (b == 0xc && (c & 0x9) == 0) {
			unsigned imm4 = bits(machInst, 19, 16);
			if (bits(imm4, 2, 0) == 0)
			{
				++instSetCounters[Unknown32]; return Unknown32;
			}
			unsigned size = 0;
			while ((imm4 & 0x1) == 0) {
				size++;
				imm4 >>= 1;
			}
			unsigned index = imm4 >> 1;
			const bool q = bits(machInst, 6) ? true : false;
			instReg.DReg[2] = vd;
			instReg.SReg[3] = vm;
			{++instSetCounters[NVdup]; return NVdup; }
		}
	}
	++instSetCounters[Unknown32];
	return Unknown32;
}
// com 
inline Aarch32Set aarch32::decodeMcrrMrrc15(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const uint8_t crm = bits(machInst, 3, 0);
	const uint8_t opc1 = bits(machInst, 7, 4);
	const MiscRegIndex miscReg = decodeCP15Reg64(crm, opc1);
	const IntRegIndex rt = (IntRegIndex)(uint8_t)bits(machInst, 15, 12);
	const IntRegIndex rt2 = (IntRegIndex)(uint8_t)bits(machInst, 19, 16);

	const bool isRead = bits(machInst, 20) ? true : false;

	switch (miscReg) {
	case MISCREG_CP15_UNIMPL:
	{++instSetCounters[FailUnimplemented]; return FailUnimplemented; }
	default:
		if (miscRegInfo[miscReg][MISCREG_WARN_NOT_FAIL]) {
			//		// deleted by WenjFu
			//		//std::string full_mnem = csprintf("%s %s",
			//		//    isRead ? "mrrc" : "mcrr", miscRegName[miscReg]);
			//		//warn("\\tinstruction '%s' unimplemented\\n", full_mnem);
			//		// delete end

			//		// Remove the warn flag and set the implemented flag. This
			//		// prevents the instruction warning a second time, it also
			//		// means the instruction is actually generated. Actually
			//		// creating the instruction to access an register that isn't
			//		// implemented sounds a bit silly, but its required to get
			//		// the correct behaviour for hyp traps and undef exceptions.
			miscRegInfo[miscReg][MISCREG_IMPLEMENTED] = true;
			miscRegInfo[miscReg][MISCREG_WARN_NOT_FAIL] = false;
		}

		if (miscRegInfo[miscReg][MISCREG_IMPLEMENTED]) {
			if (isRead) {
				if (miscRegInfo[miscReg][MISCREG_UNVERIFIABLE])
				{
					++instSetCounters[FailUnimplemented];
					return FailUnimplemented;
				}
			}
			++instSetCounters[Mcrr15];
			return Mcrr15;
		}
		else {
			{++instSetCounters[FailUnimplemented]; return FailUnimplemented; }
		}
	}
}

inline Aarch32Set aarch32::decodeNeonThreeRegistersSameLength(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const bool u = bits(machInst, 28) ? true : false;
	const uint8_t a = bits(machInst, 11, 8);
	const bool b = bits(machInst, 4) ? true : false;
	const uint8_t c = bits(machInst, 21, 20);
	const IntRegIndex vd = (IntRegIndex)(2 * (bits(machInst, 15, 12) | (bits(machInst, 22) << 4)));
	const IntRegIndex vn = (IntRegIndex)(2 * (bits(machInst, 19, 16) | (bits(machInst, 7) << 4)));
	const IntRegIndex vm = (IntRegIndex)(2 * (bits(machInst, 3, 0) | (bits(machInst, 5) << 4)));
	const unsigned size = bits(machInst, 21, 20);
	const bool q = bits(machInst, 6) ? true : false;
	instReg.DReg[2] = vd;
	instReg.SReg[3] = vn;
	instReg.SReg[4] = vm;
	if (q && ((vd & 0x1) || (vn & 0x1) || (vm & 0x1)))
	{
		++instSetCounters[Unknown32]; return Unknown32;
	}
	switch (a) {
	case 0x0:
		if (b)
		{
			++instSetCounters[Vqadd]; return Vqadd;
		}
		else {
			if (size == 3)
			{
				++instSetCounters[Unknown32]; return Unknown32;
			}
		{++instSetCounters[Vhadd]; return Vhadd; }
		}
	case 0x1:
		if (!b) {
			{++instSetCounters[Vrhadd]; return Vrhadd; }
		}
		else {
			if (u) {
				switch (c) {
				case 0:
					if (q) {
						{++instSetCounters[VeorQ]; return VeorQ; }
					}
					else {
						{++instSetCounters[VeorD]; return VeorD; }
					}
				case 1:
					if (q) {
						{++instSetCounters[VbslQ]; return VbslQ; }
					}
					else {
						{++instSetCounters[VbslD]; return VbslD; }
					}
				case 2:
					if (q) {
						{++instSetCounters[VbitQ]; return VbitQ; }
					}
					else {
						{++instSetCounters[VbitD]; return VbitD; }
					}
				case 3:
					if (q) {
						{++instSetCounters[VbifQ]; return VbifQ; }
					}
					else {
						{++instSetCounters[VbifD]; return VbifD; }
					}
				}
			}
			else {
				switch (c) {
				case 0:
					if (q) {
						{++instSetCounters[VandQ]; return VandQ; }
					}
					else {
						{++instSetCounters[VandD]; return VandD; }
					}
				case 1:
					if (q) {
						{++instSetCounters[VbicQ]; return VbicQ; }
					}
					else {
						{++instSetCounters[VbicD]; return VbicD; }
					}
				case 2:
					if (vn == vm) {
						if (q) {
							{++instSetCounters[VmovQ]; return VmovQ; }
						}
						else {
							{++instSetCounters[VmovD]; return VmovD; }
						}
					}
					else {
						if (q) {
							{++instSetCounters[VorrQ]; return VorrQ; }
						}
						else {
							{++instSetCounters[VorrD]; return VorrD; }
						}
					}
				case 3:
					if (q) {
						{++instSetCounters[VornQ]; return VornQ; }
					}
					else {
						{++instSetCounters[VornD]; return VornD; }
					}
				}
			}
		}
	case 0x2:
		if (b) {
			if (u) {
				{++instSetCounters[VqsubU]; return VqsubU; }
			}
			else {
				{++instSetCounters[VqsubS]; return VqsubS; }
			}
		}
		else {
			if (size == 3)
			{
				++instSetCounters[Unknown32]; return Unknown32;
			}
		{++instSetCounters[Vhsub]; return Vhsub; }
		}
	case 0x3:
		if (b) {
			{++instSetCounters[Vcge]; return Vcge; }
		}
		else {
			{++instSetCounters[Vcgt]; return Vcgt; }
		}
	case 0x4:
		if (b) {
			if (u) {
				{++instSetCounters[VqshlU]; return VqshlU; }
			}
			else {
				{++instSetCounters[VqshlS]; return VqshlS; }
			}
		}
		else {
			{++instSetCounters[Vshl]; return Vshl; }
		}
	case 0x5:
		if (b) {
			if (u) {
				{++instSetCounters[VqrshlU]; return VqrshlU; }
			}
			else {
				{++instSetCounters[VqrshlS]; return VqrshlS; }
			}
		}
		else {
			{++instSetCounters[Vrshl]; return Vrshl; }
		}
	case 0x6:
		if (b) {
			{++instSetCounters[Vmin]; return Vmin; }
		}
		else {
			{++instSetCounters[Vmax]; return Vmax; }
		}
	case 0x7:
		if (b) {
			{++instSetCounters[Vaba]; return Vaba; }
		}
		else {
			if (bits(machInst, 23) == 1) {
				if (q) {
					{++instSetCounters[Unknown32]; return Unknown32; }
				}
				else {
					{++instSetCounters[Vabdl]; return Vabdl; }
				}
			}
			else {
				{++instSetCounters[Vabd]; return Vabd; }
			}
		}
	case 0x8:
		if (b) {
			if (u) {
				{++instSetCounters[Vceq]; return Vceq; }
			}
			else {
				{++instSetCounters[Vtst]; return Vtst; }
			}
		}
		else {
			if (u) {
				{++instSetCounters[NVsub]; return NVsub; }
			}
			else {
				{++instSetCounters[NVadd]; return NVadd; }
			}
		}
	case 0x9:
		if (b) {
			if (u) {
				{++instSetCounters[NVmulp]; return NVmulp; }
			}
			else {
				{++instSetCounters[NVmul]; return NVmul; }
			}
		}
		else {
			if (u) {
				{++instSetCounters[NVmls]; return NVmls; }
			}
			else {
				{++instSetCounters[NVmla]; return NVmla; }
			}
		}
	case 0xa:
		if (q)
		{
			++instSetCounters[Unknown32]; return Unknown32;
		}
		if (b) {
			{++instSetCounters[Vpmin]; return Vpmin; }
		}
		else {
			{++instSetCounters[Vpmax]; return Vpmax; }
		}
	case 0xb:
		if (b) {
			if (u || q) {
				{++instSetCounters[Unknown32]; return Unknown32; }
			}
			else {
				{++instSetCounters[NVpaddD]; return NVpaddD; }
			}
		}
		else {
			if (u) {
				{++instSetCounters[Vqrdmulh]; return Vqrdmulh; }
			}
			else {
				{++instSetCounters[Vqdmulh]; return Vqdmulh; }
			}
		}
	case 0xc:
		if (b) {
			if (!u) {
				if (bits(c, 1) == 0) {
					if (q) {
						{++instSetCounters[NVfmaQF]; return NVfmaQF; }
					}
					else {
						{++instSetCounters[NVfmaDF]; return NVfmaDF; }
					}
				}
				else {
					if (q) {
						{++instSetCounters[NVfmsQF]; return NVfmsQF; }
					}
					else {
						{++instSetCounters[NVfmsDF]; return NVfmsDF; }
					}
				}
			}
		}
	{++instSetCounters[Unknown32]; return Unknown32; }
	case 0xd:
		if (b) {
			if (u) {
				if (bits(c, 1) == 0) {
					if (q) {
						{++instSetCounters[NVmulQFp]; return NVmulQFp; }
					}
					else {
						{++instSetCounters[NVmulDFp]; return NVmulDFp; }
					}
				}
				else {
					{++instSetCounters[Unknown32]; return Unknown32; }
				}
			}
			else {
				if (bits(c, 1) == 0) {
					if (q) {
						{++instSetCounters[NVmlaQFp]; return NVmlaQFp; }
					}
					else {
						{++instSetCounters[NVmlaDFp]; return NVmlaDFp; }
					}
				}
				else {
					if (q) {
						{++instSetCounters[NVmlsQFp]; return NVmlsQFp; }
					}
					else {
						{++instSetCounters[NVmlsDFp]; return NVmlsDFp; }
					}
				}
			}
		}
		else {
			if (u) {
				if (bits(c, 1) == 0) {
					if (q) {
						{++instSetCounters[VpaddQFp]; return VpaddQFp; }
					}
					else {
						{++instSetCounters[VpaddDFp]; return VpaddDFp; }
					}
				}
				else {
					if (q) {
						{++instSetCounters[VabdQFp]; return VabdQFp; }
					}
					else {
						{++instSetCounters[VabdDFp]; return VabdDFp; }
					}
				}
			}
			else {
				if (bits(c, 1) == 0) {
					if (q) {
						{++instSetCounters[VaddQFp]; return VaddQFp; }
					}
					else {
						{++instSetCounters[VaddDFp]; return VaddDFp; }
					}
				}
				else {
					if (q) {
						{++instSetCounters[VsubQFp]; return VsubQFp; }
					}
					else {
						{++instSetCounters[VsubDFp]; return VsubDFp; }
					}
				}
			}
		}
	case 0xe:
		if (b) {
			if (u) {
				if (bits(c, 1) == 0) {
					if (q) {
						{++instSetCounters[VacgeQFp]; return VacgeQFp; }
					}
					else {
						{++instSetCounters[VacgeDFp]; return VacgeDFp; }
					}
				}
				else {
					if (q) {
						{++instSetCounters[VacgtQFp]; return VacgtQFp; }
					}
					else {
						{++instSetCounters[VacgtDFp]; return VacgtDFp; }
					}
				}
			}
			else {
				{++instSetCounters[Unknown32]; return Unknown32; }
			}
		}
		else {
			if (u) {
				if (bits(c, 1) == 0) {
					if (q) {
						{++instSetCounters[VcgeQFp]; return VcgeQFp; }
					}
					else {
						{++instSetCounters[VcgeDFp]; return VcgeDFp; }
					}
				}
				else {
					if (q) {
						{++instSetCounters[VcgtQFp]; return VcgtQFp; }
					}
					else {
						{++instSetCounters[VcgtDFp]; return VcgtDFp; }
					}
				}
			}
			else {
				if (bits(c, 1) == 0) {
					if (q) {
						{++instSetCounters[VceqQFp]; return VceqQFp; }
					}
					else {
						{++instSetCounters[VceqDFp]; return VceqDFp; }
					}
				}
				else {
					{++instSetCounters[Unknown32]; return Unknown32; }
				}
			}
		}
	case 0xf:
		if (b) {
			if (u) {
				{++instSetCounters[Unknown32]; return Unknown32; }
			}
			else {
				if (bits(c, 1) == 0) {
					if (q) {
						{++instSetCounters[VrecpsQFp]; return VrecpsQFp; }
					}
					else {
						{++instSetCounters[VrecpsDFp]; return VrecpsDFp; }
					}
				}
				else {
					if (q) {
						{++instSetCounters[VrsqrtsQFp]; return VrsqrtsQFp; }
					}
					else {
						{++instSetCounters[VrsqrtsDFp]; return VrsqrtsDFp; }
					}
				}
			}
		}
		else {
			if (u) {
				if (bits(c, 1) == 0) {
					if (q) {
						{++instSetCounters[VpmaxQFp]; return VpmaxQFp; }
					}
					else {
						{++instSetCounters[VpmaxDFp]; return VpmaxDFp; }
					}
				}
				else {
					if (q) {
						{++instSetCounters[VpminQFp]; return VpminQFp; }
					}
					else {
						{++instSetCounters[VpminDFp]; return VpminDFp; }
					}
				}
			}
			else {
				if (bits(c, 1) == 0) {
					if (q) {
						{++instSetCounters[VmaxQFp]; return VmaxQFp; }
					}
					else {
						{++instSetCounters[VmaxDFp]; return VmaxDFp; }
					}
				}
				else {
					if (q) {
						{++instSetCounters[VminQFp]; return VminQFp; }
					}
					else {
						{++instSetCounters[VminDFp]; return VminDFp; }
					}
				}
			}
		}
	}
	{++instSetCounters[Unknown32]; return Unknown32; }
}

inline Aarch32Set aarch32::decodeNeonOneRegModImm(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const IntRegIndex vd = (IntRegIndex)(2 * (bits(machInst, 15, 12) | (bits(machInst, 22) << 4)));
	const bool q = bits(machInst, 6) ? true : false;
	const bool op = bits(machInst, 5) ? true : false;
	const uint8_t cmode = bits(machInst, 11, 8);
	const uint8_t imm = (bits(machInst, 28) << 7) | (bits(machInst, 18, 16) << 4) | (bits(machInst, 3, 0) << 0);
	instReg.DReg[2] = vd;
	// Check for invalid immediate encodings and return an unknown op
	// if it happens
	bool immValid = true;
	const uint64_t bigImm = 0;//deleted by WenjFu simd_modified_imm(op, cmode, imm, immValid);
	if (!immValid) {
		{++instSetCounters[Unknown32]; return Unknown32; }
	}

	if (op) {
		if (bits(cmode, 3) == 0) {
			if (bits(cmode, 0) == 0) {
				if (q)
				{
					++instSetCounters[NVmvniQ]; return NVmvniQ;
				}
				else
				{
					++instSetCounters[NVmvniD]; return NVmvniD;
				}
			}
			else {
				if (q)
				{
					++instSetCounters[NVbiciQ]; return NVbiciQ;
				}
				else
				{
					++instSetCounters[NVbiciD]; return NVbiciD;
				}
			}
		}
		else {
			if (bits(cmode, 2) == 1) {
				switch (bits(cmode, 1, 0)) {
				case 0:
				case 1:
					if (q)
					{
						++instSetCounters[NVmvniQ]; return NVmvniQ;
					}
					else
					{
						++instSetCounters[NVmvniD]; return NVmvniD;
					}
				case 2:
					if (q)
					{
						++instSetCounters[NVmoviQ]; return NVmoviQ;
					}
					else
					{
						++instSetCounters[NVmoviD]; return NVmoviD;
					}
				case 3:
					if (q)
					{
						++instSetCounters[Unknown32]; return Unknown32;
					}
					else
					{
						++instSetCounters[Unknown32]; return Unknown32;
					}
				}
			}
			else {
				if (bits(cmode, 0) == 0) {
					if (q)
					{
						++instSetCounters[NVmvniQ]; return NVmvniQ;
					}
					else
					{
						++instSetCounters[NVmvniD]; return NVmvniD;
					}
				}
				else {
					if (q)
					{
						++instSetCounters[NVbiciQ]; return NVbiciQ;
					}
					else
					{
						++instSetCounters[NVbiciD]; return NVbiciD;
					}
				}
			}
		}
	}
	else {
		if (bits(cmode, 3) == 0) {
			if (bits(cmode, 0) == 0) {
				if (q)
				{
					++instSetCounters[NVmoviQ]; return NVmoviQ;
				}
				else
				{
					++instSetCounters[NVmoviD]; return NVmoviD;
				}
			}
			else {
				if (q)
				{
					++instSetCounters[NVorriQ]; return NVorriQ;
				}
				else
				{
					++instSetCounters[NVorriD]; return NVorriD;
				}
			}
		}
		else {
			if (bits(cmode, 2) == 1) {
				if (q)
				{
					++instSetCounters[NVmoviQ]; return NVmoviQ;
				}
				else
				{
					++instSetCounters[NVmoviD]; return NVmoviD;
				}
			}
			else {
				if (bits(cmode, 0) == 0) {
					if (q)
					{
						++instSetCounters[NVmoviQ]; return NVmoviQ;
					}
					else
					{
						++instSetCounters[NVmoviD]; return NVmoviD;
					}
				}
				else {
					if (q)
					{
						++instSetCounters[NVorriQ]; return NVorriQ;
					}
					else
					{
						++instSetCounters[NVorriD]; return NVorriD;
					}
				}
			}
		}
	}
	{++instSetCounters[Unknown32]; return Unknown32; }
}

inline Aarch32Set aarch32::decodeNeonTwoRegAndShift(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const uint8_t a = bits(machInst, 11, 8);
	const bool u = bits(machInst, 28) ? true : false;
	const bool b = bits(machInst, 6) ? true : false;
	const bool l = bits(machInst, 7) ? true : false;
	const IntRegIndex vd = (IntRegIndex)(2 * (bits(machInst, 15, 12) | (bits(machInst, 22) << 4)));
	const IntRegIndex vm = (IntRegIndex)(2 * (bits(machInst, 3, 0) | (bits(machInst, 5) << 4)));
	unsigned imm6 = bits(machInst, 21, 16);
	unsigned imm = ((l ? 1 : 0) << 6) | imm6;
	unsigned size = 3;
	unsigned lShiftAmt = 0;
	unsigned bitSel;
	for (bitSel = 1 << 6; true; bitSel >>= 1) {
		if (bitSel & imm)
			break;
		else if (!size)
		{
			++instSetCounters[Unknown32]; return Unknown32;
		}
		size--;
	}
	lShiftAmt = imm6 & ~bitSel;
	unsigned rShiftAmt = 0;
	if (a != 0xe && a != 0xf) {
		if (size > 2)
			rShiftAmt = 64 - imm6;
		else
			rShiftAmt = 2 * (8 << size) - imm6;
	}
	instReg.DReg[2] = vd;
	instReg.SReg[3] = vm;
	switch (a) {
	case 0x0:
	{++instSetCounters[NVshr]; return NVshr; }
	case 0x1:
	{++instSetCounters[NVsra]; return NVsra; }
	case 0x2:
	{++instSetCounters[NVrshr]; return NVrshr; }
	case 0x3:
	{++instSetCounters[NVrsra]; return NVrsra; }
	case 0x4:
		if (u) {
			{++instSetCounters[NVsri]; return NVsri; }
		}
		else {
			{++instSetCounters[Unknown32]; return Unknown32; }
		}
	case 0x5:
		if (u) {
			{++instSetCounters[NVsli]; return NVsli; }
		}
		else {
			{++instSetCounters[NVshl]; return NVshl; }
		}
	case 0x6:
	case 0x7:
		if (u) {
			if (a == 0x6) {
				{++instSetCounters[NVqshlus]; return NVqshlus; }
			}
			else {
				{++instSetCounters[NVqshlu]; return NVqshlu; }
			}
		}
		else {
			{++instSetCounters[NVqshl]; return NVqshl; }
		}
	case 0x8:
		if (l) {
			{++instSetCounters[Unknown32]; return Unknown32; }
		}
		else if (u) {
			{++instSetCounters[NVqshruns]; return NVqshruns; }
		}
		else {
			{++instSetCounters[NVshrn]; return NVshrn; }
		}
	case 0x9:
		if (l) {
			{++instSetCounters[Unknown32]; return Unknown32; }
		}
		else if (u) {
			{++instSetCounters[NVqshrun]; return NVqshrun; }
		}
		else {
			{++instSetCounters[NVqshrn]; return NVqshrn; }
		}
	case 0xa:
		if (l || b) {
			{++instSetCounters[Unknown32]; return Unknown32; }
		}
		else {
			uint8_t imm3H = bits(machInst, 21, 19);
			if (imm3H != 1 && imm3H != 2 && imm3H != 4)
			{
				++instSetCounters[NVshll]; return NVshll;
			}
		{++instSetCounters[NVmovl]; return NVmovl; }
		}
	case 0xe:
		if (l) {
			{++instSetCounters[Unknown32]; return Unknown32; }
		}
		else {
			if (bits(imm6, 5) == 0)
			{
				++instSetCounters[Unknown32]; return Unknown32;
			}
			if (u) {
				if (b) {
					{++instSetCounters[NVcvtu2fpQ]; return NVcvtu2fpQ; }
				}
				else {
					{++instSetCounters[NVcvtu2fpD]; return NVcvtu2fpD; }
				}
			}
			else {
				if (b) {
					{++instSetCounters[NVcvts2fpQ]; return NVcvts2fpQ; }
				}
				else {
					{++instSetCounters[NVcvts2fpD]; return NVcvts2fpD; }
				}
			}
		}
	case 0xf:
		if (l) {
			{++instSetCounters[Unknown32]; return Unknown32; }
		}
		else {
			if (bits(imm6, 5) == 0)
			{
				++instSetCounters[Unknown32]; return Unknown32;
			}
			if (u) {
				if (b) {
					{++instSetCounters[NVcvt2ufxQ]; return NVcvt2ufxQ; }
				}
				else {
					{++instSetCounters[NVcvt2ufxD]; return NVcvt2ufxD; }
				}
			}
			else {
				if (b) {
					{++instSetCounters[NVcvt2sfxQ]; return NVcvt2sfxQ; }
				}
				else {
					{++instSetCounters[NVcvt2sfxD]; return NVcvt2sfxD; }
				}
			}
		}
	}
	{++instSetCounters[Unknown32]; return Unknown32; }
}

inline Aarch32Set aarch32::decodeNeonThreeRegDiffLengths(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const bool u = bits(machInst, 28) ? true : false;
	const uint8_t a = bits(machInst, 11, 8);
	const IntRegIndex vd = (IntRegIndex)(2 * (bits(machInst, 15, 12) | (bits(machInst, 22) << 4)));
	const IntRegIndex vn = (IntRegIndex)(2 * (bits(machInst, 19, 16) | (bits(machInst, 7) << 4)));
	const IntRegIndex vm = (IntRegIndex)(2 * (bits(machInst, 3, 0) | (bits(machInst, 5) << 4)));
	const unsigned size = bits(machInst, 21, 20);
	instReg.DReg[2] = vd;
	instReg.SReg[3] = vm;
	instReg.SReg[4] = vn;
	switch (a) {
	case 0x0:
	{++instSetCounters[Vaddl]; return Vaddl; }
	case 0x1:
	{++instSetCounters[Vaddw]; return Vaddw; }
	case 0x2:
	{++instSetCounters[Vsubl]; return Vsubl; }
	case 0x3:
	{++instSetCounters[Vsubw]; return Vsubw; }
	case 0x4:
		if (u) {
			{++instSetCounters[Vraddhn]; return Vraddhn; }
		}
		else {
			{++instSetCounters[Vaddhn]; return Vaddhn; }
		}
	case 0x5:
	{++instSetCounters[Vabal]; return Vabal; }
	case 0x6:
		if (u) {
			{++instSetCounters[Vrsubhn]; return Vrsubhn; }
		}
		else {
			{++instSetCounters[Vsubhn]; return Vsubhn; }
		}
	case 0x7:
		if (bits(machInst, 23)) {
			{++instSetCounters[Vabdl]; return Vabdl; }
		}
		else {
			{++instSetCounters[Vabd]; return Vabd; }
		}
	case 0x8:
	{++instSetCounters[Vmlal]; return Vmlal; }
	case 0xa:
	{++instSetCounters[Vmlsl]; return Vmlsl; }
	case 0x9:
		if (u) {
			{++instSetCounters[Unknown32]; return Unknown32; }
		}
		else {
			{++instSetCounters[Vqdmlal]; return Vqdmlal; }
		}
	case 0xb:
		if (u) {
			{++instSetCounters[Unknown32]; return Unknown32; }
		}
		else {
			{++instSetCounters[Vqdmlsl]; return Vqdmlsl; }
		}
	case 0xc:
	{++instSetCounters[Vmull]; return Vmull; }
	case 0xd:
		if (u) {
			{++instSetCounters[Unknown32]; return Unknown32; }
		}
		else {
			{++instSetCounters[Vqdmull]; return Vqdmull; }
		}
	case 0xe:
	{++instSetCounters[Vmullp]; return Vmullp; }
	}
	{++instSetCounters[Unknown32]; return Unknown32; }
}

inline Aarch32Set aarch32::decodeNeonTwoRegScalar(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const bool u = bits(machInst, 28) ? true : false;
	const uint8_t a = bits(machInst, 11, 8);
	const unsigned size = bits(machInst, 21, 20);
	const IntRegIndex vd = (IntRegIndex)(2 * (bits(machInst, 15, 12) | (bits(machInst, 22) << 4)));
	const IntRegIndex vn = (IntRegIndex)(2 * (bits(machInst, 19, 16) | (bits(machInst, 7) << 4)));
	const IntRegIndex vm = (size == 2) ? (IntRegIndex)(2 * bits(machInst, 3, 0)) : (IntRegIndex)(2 * bits(machInst, 2, 0));
	const unsigned index = (size == 2) ? (unsigned)bits(machInst, 5) : (bits(machInst, 3) | (bits(machInst, 5) << 1));
	instReg.DReg[2] = vd;
	instReg.SReg[3] = vm;
	instReg.SReg[4] = vn;
	switch (a) {
	case 0x0:
		if (u) {
			switch (size) {
			case 1:
			{++instSetCounters[VmlasQ]; return VmlasQ; }
			case 2:
			{++instSetCounters[VmlasQ]; return VmlasQ; }
			default:
			{++instSetCounters[Unknown32]; return Unknown32; }
			}
		}
		else {
			switch (size) {
			case 1:
			{++instSetCounters[VmlasD]; return VmlasD; }
			case 2:
			{++instSetCounters[VmlasD]; return VmlasD; }
			default:
			{++instSetCounters[Unknown32]; return Unknown32; }
			}
		}
	case 0x1:
		if (u)
		{
			++instSetCounters[VmlasQFp]; return VmlasQFp;
		}
		else
		{
			++instSetCounters[VmlasDFp]; return VmlasDFp;
		}
	case 0x4:
		if (u) {
			switch (size) {
			case 1:
			{++instSetCounters[VmlssQ]; return VmlssQ; }
			case 2:
			{++instSetCounters[VmlssQ]; return VmlssQ; }
			default:
			{++instSetCounters[Unknown32]; return Unknown32; }
			}
		}
		else {
			switch (size) {
			case 1:
			{++instSetCounters[VmlssD]; return VmlssD; }
			case 2:
			{++instSetCounters[VmlssD]; return VmlssD; }
			default:
			{++instSetCounters[Unknown32]; return Unknown32; }
			}
		}
	case 0x5:
		if (u)
		{
			++instSetCounters[VmlssQFp]; return VmlssQFp;
		}
		else
		{
			++instSetCounters[VmlssDFp]; return VmlssDFp;
		}
	case 0x2:
		if (u) {
			switch (size) {
			case 1:
			{++instSetCounters[Vmlals]; return Vmlals; }
			case 2:
			{++instSetCounters[Vmlals]; return Vmlals; }
			default:
			{++instSetCounters[Unknown32]; return Unknown32; }
			}
		}
		else {
			switch (size) {
			case 1:
			{++instSetCounters[Vmlals]; return Vmlals; }
			case 2:
			{++instSetCounters[Vmlals]; return Vmlals; }
			default:
			{++instSetCounters[Unknown32]; return Unknown32; }
			}
		}
	case 0x6:
		if (u) {
			switch (size) {
			case 1:
			{++instSetCounters[Vmlsls]; return Vmlsls; }
			case 2:
			{++instSetCounters[Vmlsls]; return Vmlsls; }
			default:
			{++instSetCounters[Unknown32]; return Unknown32; }
			}
		}
		else {
			switch (size) {
			case 1:
			{++instSetCounters[Vmlsls]; return Vmlsls; }
			case 2:
			{++instSetCounters[Vmlsls]; return Vmlsls; }
			default:
			{++instSetCounters[Unknown32]; return Unknown32; }
			}
		}
	case 0x3:
		if (u) {
			{++instSetCounters[Unknown32]; return Unknown32; }
		}
		else {
			switch (size) {
			case 1:
			{++instSetCounters[Vqdmlals]; return Vqdmlals; }
			case 2:
			{++instSetCounters[Vqdmlals]; return Vqdmlals; }
			default:
			{++instSetCounters[Unknown32]; return Unknown32; }
			}
		}
	case 0x7:
		if (u) {
			{++instSetCounters[Unknown32]; return Unknown32; }
		}
		else {
			switch (size) {
			case 1:
			{++instSetCounters[Vqdmlsls]; return Vqdmlsls; }
			case 2:
			{++instSetCounters[Vqdmlsls]; return Vqdmlsls; }
			default:
			{++instSetCounters[Unknown32]; return Unknown32; }
			}
		}
	case 0x8:
		if (u) {
			switch (size) {
			case 1:
			{++instSetCounters[VmulsQ]; return VmulsQ; }
			case 2:
			{++instSetCounters[VmulsQ]; return VmulsQ; }
			default:
			{++instSetCounters[Unknown32]; return Unknown32; }
			}
		}
		else {
			switch (size) {
			case 1:
			{++instSetCounters[VmulsD]; return VmulsD; }
			case 2:
			{++instSetCounters[VmulsD]; return VmulsD; }
			default:
			{++instSetCounters[Unknown32]; return Unknown32; }
			}
		}
	case 0x9:
		if (u)
		{
			++instSetCounters[VmulsQFp]; return VmulsQFp;
		}
		else
		{
			++instSetCounters[VmulsDFp]; return VmulsDFp;
		}
	case 0xa:
		if (u) {
			switch (size) {
			case 1:
			{++instSetCounters[Vmulls]; return Vmulls; }
			case 2:
			{++instSetCounters[Vmulls]; return Vmulls; }
			default:
			{++instSetCounters[Unknown32]; return Unknown32; }
			}
		}
		else {
			switch (size) {
			case 1:
			{++instSetCounters[Vmulls]; return Vmulls; }
			case 2:
			{++instSetCounters[Vmulls]; return Vmulls; }
			default:
			{++instSetCounters[Unknown32]; return Unknown32; }
			}
		}
	case 0xb:
		if (u) {
			{++instSetCounters[Unknown32]; return Unknown32; }
		}
		else {
			if (u) {
				switch (size) {
				case 1:
				{++instSetCounters[Vqdmulls]; return Vqdmulls; }
				case 2:
				{++instSetCounters[Vqdmulls]; return Vqdmulls; }
				default:
				{++instSetCounters[Unknown32]; return Unknown32; }
				}
			}
			else {
				switch (size) {
				case 1:
				{++instSetCounters[Vqdmulls]; return Vqdmulls; }
				case 2:
				{++instSetCounters[Vqdmulls]; return Vqdmulls; }
				default:
				{++instSetCounters[Unknown32]; return Unknown32; }
				}
			}
		}
	case 0xc:
		if (u) {
			switch (size) {
			case 1:
			{++instSetCounters[VqdmulhsQ]; return VqdmulhsQ; }
			case 2:
			{++instSetCounters[VqdmulhsQ]; return VqdmulhsQ; }
			default:
			{++instSetCounters[Unknown32]; return Unknown32; }
			}
		}
		else {
			switch (size) {
			case 1:
			{++instSetCounters[VqdmulhsD]; return VqdmulhsD; }
			case 2:
			{++instSetCounters[VqdmulhsD]; return VqdmulhsD; }
			default:
			{++instSetCounters[Unknown32]; return Unknown32; }
			}
		}
	case 0xd:
		if (u) {
			switch (size) {
			case 1:
			{++instSetCounters[VqrdmulhsQ]; return VqrdmulhsQ; }
			case 2:
			{++instSetCounters[VqrdmulhsQ]; return VqrdmulhsQ; }
			default:
			{++instSetCounters[Unknown32]; return Unknown32; }
			}
		}
		else {
			switch (size) {
			case 1:
			{++instSetCounters[VqrdmulhsD]; return VqrdmulhsD; }
			case 2:
			{++instSetCounters[VqrdmulhsD]; return VqrdmulhsD; }
			default:
			{++instSetCounters[Unknown32]; return Unknown32; }
			}
		}
	}
	{++instSetCounters[Unknown32]; return Unknown32; }
}

inline Aarch32Set aarch32::decodeNeonTwoRegMisc(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType){
	const uint8_t a = bits(machInst, 17, 16);
	const uint8_t b = bits(machInst, 10, 6);
	const bool q = bits(machInst, 6) ? true : false;
	const IntRegIndex vd = (IntRegIndex)(2 * (bits(machInst, 15, 12) | (bits(machInst, 22) << 4)));
	const IntRegIndex vm = (IntRegIndex)(2 * (bits(machInst, 3, 0) | (bits(machInst, 5) << 4)));
	const unsigned size = bits(machInst, 19, 18);
	instReg.DReg[2] = vd;
	instReg.SReg[3] = vm;
	switch (a) {
	case 0x0:
		switch (bits(b, 4, 1)) {
		case 0x0:
			switch (size) {
			case 0:
				if (q) {
					{++instSetCounters[NVrev64Q]; return NVrev64Q; }
				}
				else {
					{++instSetCounters[NVrev64D]; return NVrev64D; }
				}
			case 1:
				if (q) {
					{++instSetCounters[NVrev64Q]; return NVrev64Q; }
				}
				else {
					{++instSetCounters[NVrev64D]; return NVrev64D; }
				}
			case 2:
				if (q) {
					{++instSetCounters[NVrev64Q]; return NVrev64Q; }
				}
				else {
					{++instSetCounters[NVrev64D]; return NVrev64D; }
				}
			default:
			{++instSetCounters[Unknown32]; return Unknown32; }
			}
		case 0x1:
			switch (size) {
			case 0:
				if (q) {
					{++instSetCounters[NVrev32Q]; return NVrev32Q; }
				}
				else {
					{++instSetCounters[NVrev32D]; return NVrev32D; }
				}
			case 1:
				if (q) {
					{++instSetCounters[NVrev32Q]; return NVrev32Q; }
				}
				else {
					{++instSetCounters[NVrev32D]; return NVrev32D; }
				}
			default:
			{++instSetCounters[Unknown32]; return Unknown32; }
			}
		case 0x2:
			if (size != 0) {
				{++instSetCounters[Unknown32]; return Unknown32; }
			}
			else if (q) {
				{++instSetCounters[NVrev16Q]; return NVrev16Q; }
			}
			else {
				{++instSetCounters[NVrev16D]; return NVrev16D; }
			}
		case 0x4:
		{++instSetCounters[NVpaddl]; return NVpaddl; }
		case 0x5:
		{++instSetCounters[NVpaddl]; return NVpaddl; }
		case 0x8:
		{++instSetCounters[NVcls]; return NVcls; }
		case 0x9:
		{++instSetCounters[NVclz]; return NVclz; }
		case 0xa:
		{++instSetCounters[NVcnt]; return NVcnt; }
		case 0xb:
			if (q)
			{
				++instSetCounters[NVmvnQ]; return NVmvnQ;
			}
			else
			{
				++instSetCounters[NVmvnD]; return NVmvnD;
			}
		case 0xc:
		{++instSetCounters[NVpadal]; return NVpadal; }
		case 0xd:
		{++instSetCounters[NVpadal]; return NVpadal; }
		case 0xe:
		{++instSetCounters[NVqabs]; return NVqabs; }
		case 0xf:
		{++instSetCounters[NVqneg]; return NVqneg; }
		default:
		{++instSetCounters[Unknown32]; return Unknown32; }
		}
	case 0x1:
		switch (bits(b, 3, 1)) {
		case 0x0:
			if (bits(b, 4)) {
				if (q) {
					{++instSetCounters[NVcgtQFp]; return NVcgtQFp; }
				}
				else {
					{++instSetCounters[NVcgtDFp]; return NVcgtDFp; }
				}
			}
			else {
				{++instSetCounters[NVcgt]; return NVcgt; }
			}
		case 0x1:
			if (bits(b, 4)) {
				if (q) {
					{++instSetCounters[NVcgeQFp]; return NVcgeQFp; }
				}
				else {
					{++instSetCounters[NVcgeDFp]; return NVcgeDFp; }
				}
			}
			else {
				{++instSetCounters[NVcge]; return NVcge; }
			}
		case 0x2:
			if (bits(b, 4)) {
				if (q) {
					{++instSetCounters[NVceqQFp]; return NVceqQFp; }
				}
				else {
					{++instSetCounters[NVceqDFp]; return NVceqDFp; }
				}
			}
			else {
				{++instSetCounters[NVceq]; return NVceq; }
			}
		case 0x3:
			if (bits(b, 4)) {
				if (q) {
					{++instSetCounters[NVcleQFp]; return NVcleQFp; }
				}
				else {
					{++instSetCounters[NVcleDFp]; return NVcleDFp; }
				}
			}
			else {
				{++instSetCounters[NVcle]; return NVcle; }
			}
		case 0x4:
			if (bits(b, 4)) {
				if (q) {
					{++instSetCounters[NVcltQFp]; return NVcltQFp; }
				}
				else {
					{++instSetCounters[NVcltDFp]; return NVcltDFp; }
				}
			}
			else {
				{++instSetCounters[NVclt]; return NVclt; }
			}
		case 0x6:
			if (bits(machInst, 10)) {
				if (q)
				{
					++instSetCounters[NVabsQFp]; return NVabsQFp;
				}
				else
				{
					++instSetCounters[NVabsDFp]; return NVabsDFp;
				}
			}
			else {
				{++instSetCounters[NVabs]; return NVabs; }
			}
		case 0x7:
			if (bits(machInst, 10)) {
				if (q)
				{
					++instSetCounters[NVnegQFp]; return NVnegQFp;
				}
				else
				{
					++instSetCounters[NVnegDFp]; return NVnegDFp;
				}
			}
			else {
				{++instSetCounters[NVneg]; return NVneg; }
			}
		}
	case 0x2:
		switch (bits(b, 4, 1)) {
		case 0x0:
			if (q)
			{
				++instSetCounters[NVswpQ]; return NVswpQ;
			}
			else
			{
				++instSetCounters[NVswpD]; return NVswpD;
			}
		case 0x1:
		{++instSetCounters[NVtrn]; return NVtrn; }
		case 0x2:
		{++instSetCounters[NVuzp]; return NVuzp; }
		case 0x3:
		{++instSetCounters[NVzip]; return NVzip; }
		case 0x4:
			if (b == 0x8) {
				{++instSetCounters[NVmovn]; return NVmovn; }
			}
			else {
				{++instSetCounters[NVqmovuns]; return NVqmovuns; }
			}
		case 0x5:
			if (q) {
				{++instSetCounters[NVqmovun]; return NVqmovun; }
			}
			else {
				{++instSetCounters[NVqmovn]; return NVqmovn; }
			}
		case 0x6:
			if (b == 0xc) {
				{++instSetCounters[NVshll]; return NVshll; }
			}
			else {
				{++instSetCounters[Unknown32]; return Unknown32; }
			}
		case 0xc:
		case 0xe:
			if (b == 0x18) {
				if (size != 1 || (vm % 2))
				{
					++instSetCounters[Unknown32]; return Unknown32;
				}
			{++instSetCounters[NVcvts2h]; return NVcvts2h; }
			}
			else if (b == 0x1c) {
				if (size != 1 || (vd % 2))
				{
					++instSetCounters[Unknown32]; return Unknown32;
				}
			{++instSetCounters[NVcvth2s]; return NVcvth2s; }
			}
			else {
				{++instSetCounters[Unknown32]; return Unknown32; }
			}
		default:
		{++instSetCounters[Unknown32]; return Unknown32; }
		}
	case 0x3:
		if (bits(b, 4, 3) == 0x3) {
			if ((q && (vd % 2 || vm % 2)) || size != 2) {
				{++instSetCounters[Unknown32]; return Unknown32; }
			}
			else {
				if (bits(b, 2)) {
					if (bits(b, 1)) {
						if (q) {
							{++instSetCounters[NVcvt2ufxQ]; return NVcvt2ufxQ; }
						}
						else {
							{++instSetCounters[NVcvt2ufxD]; return NVcvt2ufxD; }
						}
					}
					else {
						if (q) {
							{++instSetCounters[NVcvt2sfxQ]; return NVcvt2sfxQ; }
						}
						else {
							{++instSetCounters[NVcvt2sfxD]; return NVcvt2sfxD; }
						}
					}
				}
				else {
					if (bits(b, 1)) {
						if (q) {
							{++instSetCounters[NVcvtu2fpQ]; return NVcvtu2fpQ; }
						}
						else {
							{++instSetCounters[NVcvtu2fpD]; return NVcvtu2fpD; }
						}
					}
					else {
						if (q) {
							{++instSetCounters[NVcvts2fpQ]; return NVcvts2fpQ; }
						}
						else {
							{++instSetCounters[NVcvts2fpD]; return NVcvts2fpD; }
						}
					}
				}
			}
		}
		else if ((b & 0x1a) == 0x10) {
			if (bits(b, 2)) {
				if (q) {
					{++instSetCounters[NVrecpeQFp]; return NVrecpeQFp; }
				}
				else {
					{++instSetCounters[NVrecpeDFp]; return NVrecpeDFp; }
				}
			}
			else {
				if (q) {
					{++instSetCounters[NVrecpeQ]; return NVrecpeQ; }
				}
				else {
					{++instSetCounters[NVrecpeD]; return NVrecpeD; }
				}
			}
		}
		else if ((b & 0x1a) == 0x12) {
			if (bits(b, 2)) {
				if (q) {
					{++instSetCounters[NVrsqrteQFp]; return NVrsqrteQFp; }
				}
				else {
					{++instSetCounters[NVrsqrteDFp]; return NVrsqrteDFp; }
				}
			}
			else {
				if (q) {
					{++instSetCounters[NVrsqrteQ]; return NVrsqrteQ; }
				}
				else {
					{++instSetCounters[NVrsqrteD]; return NVrsqrteD; }
				}
			}
		}
		else {
			{++instSetCounters[Unknown32]; return Unknown32; }
		}
	}
	{++instSetCounters[Unknown32]; return Unknown32; }
}