//
// Created by shenshan on 16-9-22.
// This is for instruction set decoding,
// Input a single inst and a counter pointer
// InstReg for register dependency 
// and InstType for statistic instruction type.
#include "misc_reg_info.h"
#include "decode_aarch64.h"  
#include "decode_inst_set.h"
using namespace aarch64;

Aarch64Set aarch64::decodeA64(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType) {
    ExtMachInst op0 = bits(machInst, 28, 25);
	Aarch64Set index;
    if (bits(op0, 3, 1) == 4) {          //100x
        /* decode Data processing - immediate on page C4-191 */
		index = aarch64::decodeDataProcImm(machInst, instSetCounters, instReg, instType);
    }
    else if (bits(op0, 3, 1) == 5) {    //101x
        /* Branches, exception generating and system instructions on page C4-195 */
        index = aarch64::decodeBranchExcSys(machInst, instSetCounters, instReg, instType);
        instType.isBranch = true;
    }
    else if (bits(op0, 2) && !bits(op0, 0)) {   //x1x0
        /* Loads and stores on page C4-200 */
		index = aarch64::decodeLoadsStores(machInst, instSetCounters, instReg, instType); // it has SIMD Load
    }
    else if (bits(op0, 2, 0) == 5) {        //x101
        /* Data processing - register on page C4-222 */
		index = aarch64::decodeDataProcReg(machInst, instSetCounters, instReg, instType);
    }
	/* add if (bits(op0, 2, 0) == 7) by WenjFu */
	else if (bits(op0, 2, 0) == 7){     //x111
		/* Data processing - SIMD and floating point on page C4-231 */
		index = aarch64::decodeFpAdvSIMD(machInst, instSetCounters, instReg, instType);
		instType.isFloat = true;
	}
	/* add by WenjFu */
	else{
		++instSetCounters[Unknown64];
		index = Unknown64;
	}
    /* handle the unknown instruction */
	if (index == Unknown64) {
        instType.isUnKnown = true;
        instType.isBranch = false;
		instType.isLoad = false;
		instType.isStore = false;
		instType.isSerial = false;
		instType.isFloat = false;
		instType.isMISC = false;
		instType.isCc = false;
    }
	/* added by WenjFu 11/07 */
	//for (int i = 0; i < SREG_SIZE; i++){
	//	instReg.SReg[i] += REG_NUM;
	//}
	//for (int i = 0; i < DREG_SIZE; i++){
	//	instReg.DReg[i] += REG_NUM;
	//}
	return index;
}

/* no need instType */
inline Aarch64Set aarch64::decodeDataProcImm(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType) {
    IntRegIndex rd = (IntRegIndex) (uint32_t) bits(machInst, 4, 0);
    instReg.DReg[0] = rd;
    IntRegIndex rdsp = makeSP(rd);
    IntRegIndex rdzr = makeZero(rd);
    IntRegIndex rn = (IntRegIndex) (uint32_t) bits(machInst, 9, 5);
    //instReg.SReg[0] = rn;
    IntRegIndex rnsp = makeSP(rn);

    uint8_t opc = bits(machInst, 30, 29);
    bool sf = bits(machInst, 31);
    bool n = bits(machInst, 22);
    uint8_t immr = bits(machInst, 21, 16);
    uint8_t imms = bits(machInst, 15, 10);
    switch (bits(machInst, 25, 23)) {
        case 0x0:
        case 0x1: {
            uint64_t immlo = bits(machInst, 30, 29);
            uint64_t immhi = bits(machInst, 23, 5);
            uint64_t imm = (immlo << 0) | (immhi << 2);
            if (bits(machInst, 31) == 0) {
                ++instSetCounters[AdrXImm];
                return AdrXImm;
            }
            else {
                ++instSetCounters[AdrpXImm];
                return AdrpXImm;
            }
        }
        case 0x2:
        case 0x3: {
            uint32_t imm12 = bits(machInst, 21, 10);
            uint8_t shift = bits(machInst, 23, 22);
            uint32_t imm;
            if (shift == 0x0)
                imm = imm12 << 0;
            else if (shift == 0x1)
                imm = imm12 << 12;
            else {
                ++instSetCounters[Unknown64];
                return Unknown64;
            }
            /* shen */
            instReg.SReg[0] = rn;
            switch (opc) {
                case 0x0: {
                    ++instSetCounters[AddXImmCc];
					//instType.isCc = true; // by shen 10/13
                    return AddXImmCc;
                }
                case 0x1: {
                    ++instSetCounters[AddXImm];
                    return AddXImm;
                }
                case 0x2: {
                    ++instSetCounters[SubXImm];
                    return SubXImm;
                }
                case 0x3: {
                    ++instSetCounters[SubXImmCc];
					//instType.isCc = true;  // by shen 10/13
                    return SubXImmCc;
                }
            }
        }
        case 0x4: {
            if (!sf && n) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            // len = MSB(n:NOT(imms)), len < 1 is undefined.
            uint8_t len = 0;
            if (n) {
                len = 6;
            } else if (imms == 0x3f || imms == 0x3e) {
                {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            } else {
                len = findMsbSet(imms ^ 0x3f);
            }
            // Generate r, s, and size.
            uint64_t r = bits(immr, len - 1, 0);
            uint64_t s = bits(imms, len - 1, 0);
            uint8_t size = 1 << len;
            if (s == size - 1) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            // Generate the pattern with s 1s, rotated by r, with size bits.
            uint64_t pattern = mask(s + 1);
            if (r) {
                pattern = (pattern >> r) | (pattern << (size - r));
                pattern &= mask(size);
            }
            uint8_t width = sf ? 64 : 32;
            // Replicate that to fill up the immediate.
            for (uint32_t i = 1; i < (width / size); i *= 2)
                pattern |= (pattern << (i * size));
            uint64_t imm = pattern;
            /* shen */
            instReg.SReg[0] = rn;
            switch (opc) {
                case 0x0: {
                    ++instSetCounters[AndXImm];
                    return AndXImm;

                }
                case 0x1: {
                    ++instSetCounters[OrrXImm];
                    return OrrXImm;

                }
                case 0x2: {
                    ++instSetCounters[EorXImm];
                    return EorXImm;

                }
                case 0x3: {
                    ++instSetCounters[AndXImmCc];
					instType.isCc = true;
                    return AndXImmCc;

                }
            }
        }
        case 0x5: {
            IntRegIndex rd = (IntRegIndex) (uint32_t) bits(machInst, 4, 0);
            instReg.DReg[0] = rd;
            IntRegIndex rdzr = makeZero(rd);
            uint32_t imm16 = bits(machInst, 20, 5);
            uint32_t hw = bits(machInst, 22, 21);
            switch (opc) {
                case 0x0: {
                    ++instSetCounters[Movn];
                    return Movn;

                }
                case 0x1: {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
                case 0x2: {
                    ++instSetCounters[Movz];
                    return Movz;

                }
                case 0x3: {
                    ++instSetCounters[Movk];
                    return Movk;

                }
            }
        }
        case 0x6:
            if ((sf != n) || (!sf && (bits(immr, 5) || bits(imms, 5)))) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            /* shen */
            instReg.SReg[0] = rn;
            switch (opc) {
                case 0x0: {
                    ++instSetCounters[Sbfm64];
                    return Sbfm64;

                }
                case 0x1: {
                    ++instSetCounters[Bfm64];
                    return Bfm64;

                }
                case 0x2: {
                    ++instSetCounters[Ubfm64];
                    return Ubfm64;

                }
                case 0x3: {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
        case 0x7: {
            IntRegIndex rm = (IntRegIndex) (uint8_t) bits(machInst, 20, 16);
            instReg.SReg[1] = rm;
            instReg.SReg[0] = rn;
            if (opc || bits(machInst, 21)) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[Extr64];
                return Extr64;

            }
        }
    }
    ++instSetCounters[FailUnimplemented];
    return FailUnimplemented;

}

inline Aarch64Set aarch64::decodeBranchExcSys(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType) {
    switch (bits(machInst, 30, 29)) {
        case 0x0: {

            int64_t imm = sext<26>(bits(machInst, 25, 0)) << 2;
            if (bits(machInst, 31) == 0) {
                ++instSetCounters[B64];
                return B64;

            }
            else {
                ++instSetCounters[Bl64];
                return Bl64;

            }
        }
        case 0x1: {
            IntRegIndex rt = (IntRegIndex) (uint8_t) bits(machInst, 4, 0);
            /*shen   */
            instReg.SReg[0] = rt;

            if (bits(machInst, 25) == 0) {
                int64_t imm = sext<19>(bits(machInst, 23, 5)) << 2;
                if (bits(machInst, 24) == 0) {
                    ++instSetCounters[Cbz64];
                    return Cbz64;

                }
                else {
                    ++instSetCounters[Cbnz64];
                    return Cbnz64;

                }
            } else {
                uint64_t bitmask = 0x1;
                bitmask <<= bits(machInst, 23, 19);
                int64_t imm = sext<14>(bits(machInst, 18, 5)) << 2;
                if (bits(machInst, 31))
                    bitmask <<= 32;
                if (bits(machInst, 24) == 0) {
                    ++instSetCounters[Tbz64];
                    return Tbz64;

                }
                else {
                    ++instSetCounters[Tbnz64];
                    return Tbnz64;

                }
            }
        }
        case 0x2:
            // bit 30:26=10101
            if (bits(machInst, 31) == 0) {

                if (bits(machInst, 25, 24) || bits(machInst, 4)) {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
                int64_t imm = sext<19>(bits(machInst, 23, 5)) << 2;
                /* shen */

                ConditionCode condCode =
                        (ConditionCode) (uint8_t) (bits(machInst, 3, 0));
                {
                    ++instSetCounters[BCond64];
					instType.isCc = true;
                    return BCond64;

                }
            } else if (bits(machInst, 25, 24) == 0x0) {
                if (bits(machInst, 4, 2)) {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
                uint8_t decVal = (bits(machInst, 1, 0) << 0) | (bits(machInst, 23, 21) << 2);
                /* shen */

                switch (decVal) {
                    case 0x01: {
                        ++instSetCounters[Svc64];
                        return Svc64;

                    }
                    case 0x02: {
                        ++instSetCounters[FailUnimplemented];
                        return FailUnimplemented;

                    }
                    case 0x03: {
                        ++instSetCounters[Smc64];
                        return Smc64;

                    }
                    case 0x04: {
                        ++instSetCounters[FailUnimplemented];
                        return FailUnimplemented;

                    }
                    case 0x08: {
                        ++instSetCounters[FailUnimplemented];
                        return FailUnimplemented;

                    }
                    case 0x15: {
                        ++instSetCounters[FailUnimplemented];
                        return FailUnimplemented;

                    }
                    case 0x16: {
                        ++instSetCounters[FailUnimplemented];
                        return FailUnimplemented;

                    }
                    case 0x17: {
                        ++instSetCounters[FailUnimplemented];
                        return FailUnimplemented;

                    }
                    default: {
                        ++instSetCounters[Unknown64];
                        return Unknown64;

                    }
                }
            } else if (bits(machInst, 25, 22) == 0x4) {
                // bit 31:22=1101010100
                bool l = bits(machInst, 21);
                uint8_t op0 = bits(machInst, 20, 19);
                uint8_t op1 = bits(machInst, 18, 16);
                uint8_t crn = bits(machInst, 15, 12);
                uint8_t crm = bits(machInst, 11, 8);
                uint8_t op2 = bits(machInst, 7, 5);
                IntRegIndex rt = (IntRegIndex) (uint8_t) bits(machInst, 4, 0);
                instReg.SReg[0] = rt;
                /* System  **************************************************/
                switch (op0) {
                    case 0x0:
                        if (rt != 0x1f || l) {
                            ++instSetCounters[Unknown64];
                            return Unknown64;

                        }
                        if (crn == 0x2 && op1 == 0x3) {
                            switch (op2) {
                                case 0x0: {
                                    ++instSetCounters[NopInst];
                                    return NopInst;

                                }
                                case 0x1: {
                                    ++instSetCounters[YieldInst];
                                    return YieldInst;

                                }
                                case 0x2: {
                                    ++instSetCounters[WfeInst];
                                    return WfeInst;

                                }
                                case 0x3: {
                                    ++instSetCounters[WfiInst];
                                    return WfiInst;

                                }
                                case 0x4: {
                                    ++instSetCounters[SevInst];
                                    return SevInst;

                                }
                                case 0x5: {
                                    ++instSetCounters[SevlInst];
                                    return SevlInst;

                                }
                                default: {
                                    ++instSetCounters[Unknown64];
                                    return Unknown64;

                                }
                            }
                        } else if (crn == 0x3 && op1 == 0x3) {
                            switch (op2) {
                                case 0x2: {
                                    ++instSetCounters[Clrex64];
                                    return Clrex64;

                                }
                                case 0x4: {
                                    ++instSetCounters[Dsb64];
                                    instType.isSerial = 1;
									return Dsb64;
                                }
                                case 0x5: {
                                    ++instSetCounters[Dmb64];
                                    instType.isSerial = 1;
									return Dmb64;

                                }
                                case 0x6: {
                                    ++instSetCounters[Isb64];
                                    instType.isSerial = 1;
									return Isb64;
                                }
                                default: {
                                    ++instSetCounters[Unknown64];
                                    return Unknown64;
                                }
                            }
                        } else if (crn == 0x4) {
                            // MSR immediate
                            switch (op1 << 3 | op2) {
                                case 0x5:
                                    // SP
                                    ++instSetCounters[MsrSP64];
                                    return MsrSP64;

                                case 0x1e:
                                    // DAIFSet
                                    ++instSetCounters[MsrDAIFSet64];
                                    return MsrDAIFSet64;

                                case 0x1f:
                                    // DAIFClr
                                    ++instSetCounters[MsrDAIFClr64];
                                    return MsrDAIFClr64;

                                default:
                                    ++instSetCounters[Unknown64];
                                    return Unknown64;

                            }
                        } else {
                            ++instSetCounters[Unknown64];
                            return Unknown64;

                        }
                        break;
                    case 0x1:
                    case 0x2:
                    case 0x3: {
                        // bit 31:22=1101010100, 20:19=11
                        bool read = l;
                        MiscRegIndex miscReg = decodeAArch64SysReg(op0, op1, crn, crm, op2);
                        if (read) {
                            if ((miscReg == MISCREG_DC_CIVAC_Xt) ||
                                (miscReg == MISCREG_DC_CVAC_Xt) ||
                                (miscReg == MISCREG_DC_ZVA_Xt)) {
                                {
                                    ++instSetCounters[Unknown64];
                                    return Unknown64;

                                }
                            }
                        }
                        // Check for invalid registers
                        if (miscReg == MISCREG_UNKNOWN) {  // by shen 11/23
                            {
                                ++instSetCounters[Mrs64];
								return Mrs64;

                            }
                        } else if (miscRegInfo[miscReg][MISCREG_IMPLEMENTED]) {
                            if (miscReg == MISCREG_NZCV) {
                                if (read) {
                                    ++instSetCounters[MrsNZCV64];
                                    return MrsNZCV64;

                                }
                                else {
                                    ++instSetCounters[MsrNZCV64];
                                    return MsrNZCV64;

                                }
                            }
                            if (miscReg == MISCREG_DC_ZVA_Xt && !read) {
                                ++instSetCounters[Dczva];
                                return Dczva;

                            }
                            /* shen changed on 9/25 11:33 */
                            if (read) {
                                ++instSetCounters[Mrs64];
                                return Mrs64;

                            } else {
                                ++instSetCounters[Msr64];
                                return Msr64;

                            }
                        } else if (miscRegInfo[miscReg][MISCREG_WARN_NOT_FAIL]) {
                            /* shen changed on 9/25 11:37 */
                            ++instSetCounters[WarnUnimplemented];
                            return WarnUnimplemented;

                        } else {
                            ++instSetCounters[FailUnimplemented];
                            return FailUnimplemented;

                        }
                    }
                        break;
                }
            } else if (bits(machInst, 25) == 0x1) {
                uint8_t opc = bits(machInst, 24, 21);
                uint8_t op2 = bits(machInst, 20, 16);
                uint8_t op3 = bits(machInst, 15, 10);
                IntRegIndex rn = (IntRegIndex) (uint8_t) bits(machInst, 9, 5);
                /* shen */
                instReg.SReg[0] = rn;

                uint8_t op4 = bits(machInst, 4, 0);
                if (op2 != 0x1f || op3 != 0x0 || op4 != 0x0) {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
                switch (opc) {
                    case 0x0: {
                        ++instSetCounters[Br64];
                        return Br64;

                    }
                    case 0x1: {
                        ++instSetCounters[Blr64];
                        return Blr64;

                    }
                    case 0x2: {
                        ++instSetCounters[Ret64];
                        return Ret64;

                    }
                    case 0x4:
                        if (rn != 0x1f) {
                            ++instSetCounters[Unknown64];
                            return Unknown64;

                        }
                        ++instSetCounters[Eret64];
                        return Eret64;

                    case 0x5:
                        if (rn != 0x1f) {
                            ++instSetCounters[Unknown64];
                            return Unknown64;

                        }
                        ++instSetCounters[FailUnimplemented];
                        return FailUnimplemented;

                }
            }
        default:
            ++instSetCounters[Unknown64];
            return Unknown64;

    }
    ++instSetCounters[FailUnimplemented];
    return FailUnimplemented;

}

inline Aarch64Set aarch64::decodeLoadsStores(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType) {
    // bit 27,25=10
    switch (bits(machInst, 29, 28)) {
        case 0x0:
            if (bits(machInst, 26) == 0) {
                if (bits(machInst, 24) != 0) {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
                IntRegIndex rt = (IntRegIndex) (uint8_t) bits(machInst, 4, 0);
                IntRegIndex rn = (IntRegIndex) (uint8_t) bits(machInst, 9, 5);
                IntRegIndex rnsp = makeSP(rn);
                IntRegIndex rt2 = (IntRegIndex) (uint8_t) bits(machInst, 14, 10);
                IntRegIndex rs = (IntRegIndex) (uint8_t) bits(machInst, 20, 16);
                uint8_t opc = (bits(machInst, 15) << 0) |
                              (bits(machInst, 23, 21) << 1);
                uint8_t size = bits(machInst, 31, 30);
                /* rn is always source regeister */
                instReg.SReg[0] = rn;

                switch (opc) {
                    case 0x0:
                        /* shen */
                        instType.isStore = 1;
                        /* shen */
                        instReg.DReg[0] = rs;
                        instReg.SReg[1] = rt;
                        switch (size) {
                            case 0x0: {
                                ++instSetCounters[STXRB64];
                                return STXRB64;

                            }
                            case 0x1: {
                                ++instSetCounters[STXRH64];
                                return STXRH64;

                            }
                            case 0x2: {
                                ++instSetCounters[STXRW64];
                                return STXRW64;

                            }
                            case 0x3: {
                                ++instSetCounters[STXRX64];
                                return STXRX64;

                            }
                        }
                    case 0x1:
                        /* shen */
                        instType.isStore = 1;
                        /* shen */
                        instReg.DReg[0] = rs;
                        instReg.SReg[1] = rt;
                        switch (size) {
                            case 0x0: {
                                ++instSetCounters[STLXRB64];
                                return STLXRB64;

                            }
                            case 0x1: {
                                ++instSetCounters[STLXRH64];
                                return STLXRH64;

                            }
                            case 0x2: {
                                ++instSetCounters[STLXRW64];
                                return STLXRW64;

                            }
                            case 0x3: {
                                ++instSetCounters[STLXRX64];
                                return STLXRX64;

                            }
                        }
                    case 0x2:
                        /* shen */
                        instType.isStore = 1;
                        /* shen */
                        instReg.DReg[0] = rs;
                        instReg.SReg[1] = rt;
                        instReg.SReg[2] = rt2;
                        switch (size) {
                            case 0x0:
                            case 0x1: {
                                ++instSetCounters[Unknown64];
                                return Unknown64;

                            }
                            case 0x2: {
                                ++instSetCounters[STXPW64];
                                return STXPW64;

                            }
                            case 0x3: {
                                ++instSetCounters[STXPX64];
                                return STXPX64;

                            }
                        }

                    case 0x3:
                        /* shen */
                        instType.isStore = 1;
                        /* shen */
                        instReg.DReg[0] = rs;
                        instReg.SReg[1] = rt;
                        instReg.SReg[2] = rt2;
                        switch (size) {
                            case 0x0:
                            case 0x1: {
                                ++instSetCounters[Unknown64];
                                return Unknown64;

                            }
                            case 0x2: {
                                ++instSetCounters[STLXPW64];
                                return STLXPW64;

                            }
                            case 0x3: {
                                ++instSetCounters[STLXPX64];
                                return STLXPX64;

                            }
                        }

                    case 0x4:
                        /* shen */
                        instType.isLoad = 1;
                        instReg.DReg[0] = rt;
                        switch (size) {
                            case 0x0: {
                                ++instSetCounters[LDXRB64];
                                return LDXRB64;

                            }
                            case 0x1: {
                                ++instSetCounters[LDXRH64];
                                return LDXRH64;

                            }
                            case 0x2: {
                                ++instSetCounters[LDXRW64];
                                return LDXRW64;

                            }
                            case 0x3: {
                                ++instSetCounters[LDXRX64];
                                return LDXRX64;

                            }
                        }
                    case 0x5:
                        /* shen */
                        instType.isLoad = 1;
                        instReg.DReg[0] = rt;
                        switch (size) {
                            case 0x0: {
                                ++instSetCounters[LDAXRB64];
                                return LDAXRB64;

                            }
                            case 0x1: {
                                ++instSetCounters[LDAXRH64];
                                return LDAXRH64;

                            }
                            case 0x2: {
                                ++instSetCounters[LDAXRW64];
                                return LDAXRW64;

                            }
                            case 0x3: {
                                ++instSetCounters[LDAXRX64];
                                return LDAXRX64;

                            }
                        }
                    case 0x6:
                        /* shen */
                        instType.isLoad = 1;
                        instReg.DReg[0] = rt;
                        instReg.DReg[1] = rt2;
                        switch (size) {
                            case 0x0:
                            case 0x1: {
                                ++instSetCounters[Unknown64];
                                return Unknown64;

                            }
                            case 0x2: {
                                ++instSetCounters[LDXPW64];
                                return LDXPW64;

                            }
                            case 0x3: {
                                ++instSetCounters[LDXPX64];
                                return LDXPX64;

                            }
                        }

                    case 0x7:
                        /* shen */
                        instType.isLoad = 1;
                        instReg.DReg[0] = rt;
                        instReg.DReg[1] = rt2;
                        switch (size) {
                            case 0x0:
                            case 0x1: {
                                ++instSetCounters[Unknown64];
                                return Unknown64;

                            }
                            case 0x2: {
                                ++instSetCounters[LDAXPW64];
                                return LDAXPW64;

                            }
                            case 0x3: {
                                ++instSetCounters[LDAXPX64];
                                return LDAXPX64;

                            }
                        }

                    case 0x9:
                        /* shen */
                        instType.isStore = 1;
                        instReg.SReg[1] = rt;
                        switch (size) {
                            case 0x0: {
                                ++instSetCounters[STLRB64];
                                return STLRB64;

                            }
                            case 0x1: {
                                ++instSetCounters[STLRH64];
                                return STLRH64;

                            }
                            case 0x2: {
                                ++instSetCounters[STLRW64];
                                return STLRW64;

                            }
                            case 0x3: {
                                ++instSetCounters[STLRX64];
                                return STLRX64;

                            }
                        }
                    case 0xd:
                        /* shen */
                        instType.isLoad = 1;
                        instReg.SReg[1] = rt;
                        switch (size) {
                            case 0x0: {
                                ++instSetCounters[LDARB64];
                                return LDARB64;

                            }
                            case 0x1: {
                                ++instSetCounters[LDARH64];
                                return LDARH64;

                            }
                            case 0x2: {
                                ++instSetCounters[LDARW64];
                                return LDARW64;

                            }
                            case 0x3: {
                                ++instSetCounters[LDARX64];
                                return LDARX64;

                            }
                        }
                    default: {
                        ++instSetCounters[Unknown64];
                        return Unknown64;

                    }
                }
            } else if (bits(machInst, 31)) {
                {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            } else {
                /* shen changed on 9/25 11:45. definition at : neon64.isa line 2520*/
                return aarch64::decodeNeonMem(machInst, instSetCounters, instReg);

            }
        case 0x1: {
            if (bits(machInst, 24) != 0) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            uint8_t switchVal = (bits(machInst, 26) << 0) |
                                (bits(machInst, 31, 30) << 1);
            int64_t imm = sext<19>(bits(machInst, 23, 5)) << 2;
            IntRegIndex rt = (IntRegIndex) (uint32_t) bits(machInst, 4, 0);
            instType.isLoad = 1;
            switch (switchVal) {
                case 0x0:
                    /*shen */
                    instReg.DReg[0] = rt;
                    {
                        ++instSetCounters[LDRWL64_LIT];
                        return LDRWL64_LIT;

                    }
                case 0x1:
                    instReg.DReg[2] = rt;
                    {
                        ++instSetCounters[LDRSFP64_LIT];
                        return LDRSFP64_LIT;

                    }
                case 0x2:
                    /*shen */
                    instReg.DReg[0] = rt;
                    {
                        ++instSetCounters[LDRXL64_LIT];
                        return LDRXL64_LIT;

                    }
                case 0x3:
                    instReg.DReg[2] = rt;
                    {
                        ++instSetCounters[LDRDFP64_LIT];
                        return LDRDFP64_LIT;

                    }
                case 0x4:
                    /*shen */
                    instReg.DReg[0] = rt;
                    {
                        ++instSetCounters[LDRSWL64_LIT];
                        return LDRSWL64_LIT;

                    }
                case 0x5:
                    instReg.DReg[2] = rt;
                    {
                        ++instSetCounters[BigFpMemLitLd];
                        return BigFpMemLitLd;

                    } /* ldr */
                case 0x6:
                    instReg.SReg[0] = rt;
                    {
                        ++instSetCounters[PRFM64_LIT];
                        return PRFM64_LIT;

                    }
                default: {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
        }
        case 0x2: {
            uint8_t opc = bits(machInst, 31, 30);
            if (opc >= 3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            uint32_t size = 0;
            bool fp = bits(machInst, 26);
            bool load = bits(machInst, 22);
            if (fp) {
                size = 4 << opc;
            } else {
                if ((opc == 1) && !load) {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
                size = (opc == 0 || opc == 1) ? 4 : 8;
            }

            //++instSetCounters[LdpStp];
            // shen changed on 9/25 11:50
            uint8_t type = bits(machInst, 24, 23);
            int64_t imm = sext<7>(bits(machInst, 21, 15)) * size;

            IntRegIndex rn = (IntRegIndex) (uint8_t) bits(machInst, 9, 5);
            IntRegIndex rt = (IntRegIndex) (uint8_t) bits(machInst, 4, 0);
            IntRegIndex rt2 = (IntRegIndex) (uint8_t) bits(machInst, 14, 10);
            bool noAlloc = (type == 0);
            bool signExt = !noAlloc && !fp && opc == 1;
            //PairMemOp::AddrMode mode;
            const char *mnemonic = NULL;
            if (load) {
                /* shen */
                instReg.SReg[0] = rn;
                instReg.DReg[0] = rt;
                instReg.DReg[1] = rt2;
                instType.isLoad = 1;
                if (noAlloc) {
                    //mnemonic = "ldnp";
                    ++instSetCounters[Ldnp];
                    return Ldnp;

                }
                else if (signExt)
                    //mnemonic = "ldpsw";
                {
                    ++instSetCounters[Ldpsw];
                    return Ldpsw;

                }
                else
                    //mnemonic = "ldp";
                {
                    ++instSetCounters[Ldp];
                    return Ldp;

                }
            } else {
                /* shen */
                instReg.SReg[0] = rn;
                instReg.SReg[1] = rt;
                instReg.SReg[2] = rt2;
                instType.isStore = 1;
                if (noAlloc)
                    //mnemonic = "stnp";
                {
                    ++instSetCounters[Stnp];
                    return Stnp;

                }
                else
                    //mnemonic = "stp";
                {
                    ++instSetCounters[Stp];
                    return Stp;

                }
            }

            //return new LdpStp(mnemonic, machInst, size, fp, load, noAlloc, signExt, false, false, imm, mode, rn, rt, rt2);

        }
            // bit 29:27=111, 25=0
        case 0x3: {
            uint8_t switchVal = (bits(machInst, 23, 22) << 0) |
                                (bits(machInst, 26) << 2) |
                                (bits(machInst, 31, 30) << 3);
            if (bits(machInst, 24) == 1) {
                uint64_t imm12 = bits(machInst, 21, 10);
                IntRegIndex rt = (IntRegIndex) (uint32_t) bits(machInst, 4, 0);
                IntRegIndex rn = (IntRegIndex) (uint32_t) bits(machInst, 9, 5);
                IntRegIndex rnsp = makeSP(rn);
                /* shen */
                instReg.SReg[0] = rn;

                switch (switchVal) {
                    /* shen   */
                    case 0x00: {
                        ++instSetCounters[STRB64_IMM];
                        instType.isStore = 1;
                        instReg.SReg[1] = rt;
                        return STRB64_IMM;
                    }
                    case 0x01: {
                        ++instSetCounters[LDRB64_IMM];
                        instType.isLoad = 1;
                        instReg.DReg[0] = rt;
                        return LDRB64_IMM;
                    }
                    case 0x02: {
                        ++instSetCounters[LDRSBX64_IMM];
                        instType.isLoad = 1;
                        instReg.DReg[0] = rt;
                        return LDRSBX64_IMM;
                    }
                    case 0x03: {
                        ++instSetCounters[LDRSBW64_IMM];
                        instType.isLoad = 1;
                        instReg.DReg[0] = rt;
                        return LDRSBW64_IMM;
                    }
                    case 0x04: {
                        ++instSetCounters[STRBFP64_IMM];
                        instType.isStore = 1;
                        instReg.SReg[4] = rt;
                        return STRBFP64_IMM;
                    }
                    case 0x05: {
                        ++instSetCounters[LDRBFP64_IMM];
                        instType.isLoad = 1;
                        instReg.DReg[2] = rt;
                        return LDRBFP64_IMM;
                    }
                    case 0x06:
                        ++instSetCounters[BigFpMemImmSt];
                        instType.isStore = 1;
                        instReg.SReg[4] = rt;
                        return BigFpMemImmSt;
                        /* fp store */
                    case 0x07: {
                        ++instSetCounters[BigFpMemImmLd];
                        instType.isLoad = 1;
                        instReg.DReg[2] = rt;
                        return BigFpMemImmLd;

                    }
                    case 0x08: {
                        ++instSetCounters[STRH64_IMM];
                        instType.isStore = 1;
                        instReg.SReg[1] = rt;
						return STRH64_IMM;
                    }
                    case 0x09: {
                        ++instSetCounters[LDRH64_IMM];
                        instType.isLoad = 1;
                        instReg.DReg[0] = rt;
						return LDRH64_IMM;
                    }
                    case 0x0a: {
                        ++instSetCounters[LDRSHX64_IMM];
                        instType.isLoad = 1;
                        instReg.DReg[0] = rt;
						return LDRSHX64_IMM;
                    }
                    case 0x0b: {
                        ++instSetCounters[LDRSHW64_IMM];
                        instType.isLoad = 1;
                        instReg.DReg[0] = rt;
						return LDRSHW64_IMM;
                    }
                    case 0x0c: {
                        ++instSetCounters[STRHFP64_IMM];
                        instType.isStore = 1;
                        instReg.SReg[4] = rt;
						return STRHFP64_IMM;
                    }
                    case 0x0d: {
                        ++instSetCounters[LDRHFP64_IMM];
                        instType.isLoad = 1;
                        instReg.DReg[2] = rt;
						return LDRHFP64_IMM;
                    }
                    case 0x10: {
                        ++instSetCounters[STRW64_IMM];
                        instType.isStore = 1;
                        instReg.SReg[1] = rt;
						return STRW64_IMM;
                    }
                    case 0x11: {
                        ++instSetCounters[LDRW64_IMM];
                        instType.isLoad = 1;
                        instReg.DReg[0] = rt;
                        return LDRW64_IMM;
                    }
                    case 0x12: {
                        ++instSetCounters[LDRSW64_IMM];
                        instType.isLoad = 1;
                        instReg.DReg[0] = rt;
                        return LDRSW64_IMM;
                    }
                    case 0x14: {
                        ++instSetCounters[STRSFP64_IMM];

                        instType.isStore = 1;
                        instReg.SReg[4] = rt;
                        return STRSFP64_IMM;

                    }
                    case 0x15: {
                        ++instSetCounters[LDRSFP64_IMM];
                        instType.isLoad = 1;
                        instReg.DReg[2] = rt;
						return LDRSFP64_IMM;
                    }
                    case 0x18: {
                        ++instSetCounters[STRX64_IMM];
                        instType.isStore = 1;
                        instReg.SReg[1] = rt;
						return STRX64_IMM;
                    }
                    case 0x19: {
                        ++instSetCounters[LDRX64_IMM];
                        instType.isLoad = 1;
                        instReg.DReg[0] = rt;
						return LDRX64_IMM;
                    }
                    case 0x1a: {
                        ++instSetCounters[PRFM64_IMM];
                        instType.isLoad = 1;
                        instReg.SReg[1] = rt;
						return PRFM64_IMM;
                    }
                    case 0x1c: {
                        ++instSetCounters[STRDFP64_IMM];
                        instType.isStore = 1;
                        instReg.SReg[4] = rt;
						return STRDFP64_IMM;
                    }
                    case 0x1d: {
                        ++instSetCounters[LDRDFP64_IMM];
                        instType.isLoad = 1;
                        instReg.DReg[2] = rt;
						return LDRDFP64_IMM;
                    }
                    default: {
                        ++instSetCounters[Unknown64];
                        return Unknown64;
                    }
                }
            } else if (bits(machInst, 21) == 1) {
                if (bits(machInst, 11, 10) != 0x2) {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
                if (!bits(machInst, 14)) {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
                IntRegIndex rt = (IntRegIndex) (uint32_t) bits(machInst, 4, 0);
                IntRegIndex rn = (IntRegIndex) (uint32_t) bits(machInst, 9, 5);
                IntRegIndex rnsp = makeSP(rn);
                IntRegIndex rm = (IntRegIndex) (uint32_t) bits(machInst, 20, 16);
                /*  shen */
                instReg.SReg[0] = rn;
                instReg.SReg[1] = rm;
                ArmExtendType type =
                        (ArmExtendType) (uint32_t) bits(machInst, 15, 13);
                uint8_t s = bits(machInst, 12);
                switch (switchVal) {
                    case 0x00: {
                        ++instSetCounters[STRB64_REG];
                        instType.isStore = 1;
                        instReg.SReg[2] = rt;
                        return STRB64_REG;
                    }
                    case 0x01: {
                        ++instSetCounters[LDRB64_REG];
                        instType.isLoad = 1;
                        instReg.DReg[0] = rt;
                        return LDRB64_REG;
                    }
                    case 0x02: {
                        ++instSetCounters[LDRSBX64_REG];
                        instType.isLoad = 1;
                        instReg.DReg[0] = rt;
                        return LDRSBX64_REG;
                    }
                    case 0x03: {
                        ++instSetCounters[LDRSBW64_REG];
                        instType.isLoad = 1;
                        instReg.DReg[0] = rt;
                        return LDRSBW64_REG;
                    }
                    case 0x04: {
                        ++instSetCounters[STRBFP64_REG];
                        instType.isStore = 1;
                        instReg.SReg[5] = rt;
                        return STRBFP64_REG;
                    }
                    case 0x05: {
                        ++instSetCounters[LDRBFP64_REG];
                        instType.isLoad = 1;
                        instReg.DReg[2] = rt;
                        return LDRBFP64_REG;
                    }
                    case 0x6: {
                        ++instSetCounters[BigFpMemRegSt];
                        instType.isStore = 1;
                        instReg.SReg[5] = rt;
                        return BigFpMemRegSt;
                    }
                    case 0x7: {
                        ++instSetCounters[BigFpMemRegLd];
                        instType.isLoad = 1;
                        instReg.DReg[2] = rt;
                        return BigFpMemRegLd;
                    }
                    case 0x08: {
                        ++instSetCounters[STRH64_REG];
                        instType.isStore = 1;
                        instReg.SReg[2] = rt;
                        return STRH64_REG;
                    }
                    case 0x09: {
                        ++instSetCounters[LDRH64_REG];
                        instType.isLoad = 1;
                        instReg.DReg[0] = rt;
                        return LDRH64_REG;
                    }
                    case 0x0a: {
                        ++instSetCounters[LDRSHX64_REG];
                        instType.isLoad = 1;
                        instReg.DReg[0] = rt;
                        return LDRSHX64_REG;
                    }
                    case 0x0b: {
                        ++instSetCounters[LDRSHW64_REG];
                        instType.isLoad = 1;
                        instReg.DReg[0] = rt;
                        return LDRSHW64_REG;
                    }
                    case 0x0c: {
                        ++instSetCounters[STRHFP64_REG];
                        instType.isStore = 1;
                        instReg.SReg[5] = rt;
                        return STRHFP64_REG;
                    }
                    case 0x0d: {
                        ++instSetCounters[LDRHFP64_REG];
                        instType.isLoad = 1;
                        instReg.DReg[2] = rt;
                        return LDRHFP64_REG;
                    }
                    case 0x10: {
                        ++instSetCounters[STRW64_REG];
                        instType.isStore = 1;
                        instReg.SReg[2] = rt;
                        return STRW64_REG;
                    }
                    case 0x11: {
                        ++instSetCounters[LDRW64_REG];
                        instType.isLoad = 1;
                        instReg.DReg[0] = rt;
                        return LDRW64_REG;
                    }
                    case 0x12: {
                        ++instSetCounters[LDRSW64_REG];
                        instType.isLoad = 1;
                        instReg.DReg[0] = rt;
                        return LDRSW64_REG;
                    }
                    case 0x14: {
                        ++instSetCounters[STRSFP64_REG];
                        instType.isStore = 1;
                        instReg.SReg[5] = rt;
                        return STRSFP64_REG;
                    }
                    case 0x15: {
                        ++instSetCounters[LDRSFP64_REG];
                        instType.isLoad = 1;
                        instReg.DReg[2] = rt;
                        return LDRSFP64_REG;
                    }
                    case 0x18: {
                        ++instSetCounters[STRX64_REG];
                        instType.isStore = 1;
                        instReg.SReg[2] = rt;
                        return STRX64_REG;
                    }
                    case 0x19: {
                        ++instSetCounters[LDRX64_REG];
                        instType.isLoad = 1;
                        instReg.DReg[0] = rt;
                        return LDRX64_REG;
                    }
                    case 0x1a: {
                        ++instSetCounters[PRFM64_REG];
                        instType.isLoad = 1;
                        instReg.SReg[2] = rt;
                        return PRFM64_REG;
                    }
                    case 0x1c: {
                        ++instSetCounters[STRDFP64_REG];
                        instType.isStore = 1;
                        instReg.SReg[5] = rt;
                        return STRDFP64_REG;
                    }
                    case 0x1d: {
                        ++instSetCounters[LDRDFP64_REG];
                        instType.isLoad = 1;
                        instReg.DReg[2] = rt;
                        return LDRDFP64_REG;
                    }
                    default: {
                        ++instSetCounters[Unknown64];
                        return Unknown64;

                    }
                }
            } else {
                // bit 29:27=111, 25:24=00, 21=0
                switch (bits(machInst, 11, 10)) {
                    case 0x0: {
                        IntRegIndex rt = (IntRegIndex) (uint32_t) bits(machInst, 4, 0);
                        IntRegIndex rn = (IntRegIndex) (uint32_t) bits(machInst, 9, 5);
                        IntRegIndex rnsp = makeSP(rn);
                        /* rn always source register */
                        instReg.SReg[0] = rn;

                        uint64_t imm = sext<9>(bits(machInst, 20, 12));
                        switch (switchVal) {
                            case 0x00: {
                                ++instSetCounters[STURB64_IMM];
                                instType.isStore = 1;
                                instReg.SReg[1] = rt;
                                return STURB64_IMM;
                            }
                            case 0x01: {
                                ++instSetCounters[LDURB64_IMM];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDURB64_IMM;
                            }
                            case 0x02: {
                                ++instSetCounters[LDURSBX64_IMM];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDURSBX64_IMM;
                            }
                            case 0x03: {
                                ++instSetCounters[LDURSBW64_IMM];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDURSBW64_IMM;
                            }
                            case 0x04: {
                                ++instSetCounters[STURBFP64_IMM];
                                instType.isStore = 1;
                                instReg.SReg[4] = rt;
                                return STURBFP64_IMM;
                            }
                            case 0x05: {
                                ++instSetCounters[LDURBFP64_IMM];
                                instType.isLoad = 1;
                                instReg.DReg[2] = rt;
                                return LDURBFP64_IMM;
                            }
                            case 0x06:
                                ++instSetCounters[BigFpMemImmSt];
                                instType.isStore = 1;
                                instReg.SReg[4] = rt;
                                return BigFpMemImmSt;
                            case 0x07:
                                ++instSetCounters[BigFpMemImmLd];
                                instType.isLoad = 1;
                                instReg.DReg[2] = rt;
                                return BigFpMemImmLd;
                            case 0x08: {
                                ++instSetCounters[STURH64_IMM];
                                instType.isStore = 1;
                                instReg.SReg[1] = rt;
                                return STURH64_IMM;
                            }
                            case 0x09: {
                                ++instSetCounters[LDURH64_IMM];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDURH64_IMM;
                            }
                            case 0x0a: {
                                ++instSetCounters[LDURSHX64_IMM];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDURSHX64_IMM;
                            }
                            case 0x0b: {
                                ++instSetCounters[LDURSHW64_IMM];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDURSHW64_IMM;
                            }
                            case 0x0c: {
                                ++instSetCounters[STURHFP64_IMM];
                                instType.isStore = 1;
                                instReg.SReg[4] = rt;
                                return STURHFP64_IMM;
                            }
                            case 0x0d: {
                                ++instSetCounters[LDURHFP64_IMM];
                                instType.isLoad = 1;
                                instReg.DReg[2] = rt;
                                return LDURHFP64_IMM;
                            }
                            case 0x10: {
                                ++instSetCounters[STURW64_IMM];
                                instType.isStore = 1;
                                instReg.SReg[1] = rt;
                                return STURW64_IMM;
                            }
                            case 0x11: {
                                ++instSetCounters[LDURW64_IMM];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDURW64_IMM;
                            }
                            case 0x12: {
                                ++instSetCounters[LDURSW64_IMM];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDURSW64_IMM;
                            }
                            case 0x14: {
                                ++instSetCounters[STURSFP64_IMM];
                                instType.isStore = 1;
                                instReg.SReg[4] = rt;
                                return STURSFP64_IMM;
                            }
                            case 0x15: {
                                ++instSetCounters[LDURSFP64_IMM];
                                instType.isLoad = 1;
                                instReg.DReg[2] = rt;
                                return LDURSFP64_IMM;
                            }
                            case 0x18: {
                                ++instSetCounters[STURX64_IMM];
                                instType.isStore = 1;
                                instReg.SReg[1] = rt;
                                return STURX64_IMM;
                            }
                            case 0x19: {
                                ++instSetCounters[LDURX64_IMM];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDURX64_IMM;
                            }
                            case 0x1a: {
                                ++instSetCounters[PRFUM64_IMM];
                                instType.isLoad = 1;
                                instReg.SReg[1] = rt;
                                return PRFUM64_IMM;
                            }
                            case 0x1c: {
                                ++instSetCounters[STURDFP64_IMM];
                                instType.isStore = 1;
                                instReg.SReg[4] = rt;
                                return STURDFP64_IMM;
                            }
                            case 0x1d: {
                                ++instSetCounters[LDURDFP64_IMM];
                                instType.isLoad = 1;
                                instReg.DReg[2] = rt;
                                return LDURDFP64_IMM;
                            }
                            default: {
                                ++instSetCounters[Unknown64];
                                return Unknown64;

                            }
                        }
                    }
                        // bit 29:27=111, 25:24=00, 21=0, 11:10=01
                    case 0x1: {
                        IntRegIndex rt =
                                (IntRegIndex) (uint32_t) bits(machInst, 4, 0);
                        IntRegIndex rn =
                                (IntRegIndex) (uint32_t) bits(machInst, 9, 5);
                        IntRegIndex rnsp = makeSP(rn);
                        /* rn is source register */
                        instReg.SReg[0] = rn;
                        uint64_t imm = sext<9>(bits(machInst, 20, 12));
                        switch (switchVal) {
                            case 0x00: {
                                ++instSetCounters[STRB64_POST];
                                instType.isStore = 1;
                                instReg.SReg[1] = rt;
                                return STRB64_POST;
                            }
                            case 0x01: {
                                ++instSetCounters[LDRB64_POST];
                                instType.isLoad = 1;;
                                instReg.DReg[0] = rt;
                                return LDRB64_POST;
                            }
                            case 0x02: {
                                ++instSetCounters[LDRSBX64_POST];
                                instType.isLoad = 1;;
                                instReg.DReg[0] = rt;
                                return LDRSBX64_POST;
                            }
                            case 0x03: {
                                ++instSetCounters[LDRSBW64_POST];
                                instType.isLoad = 1;;
                                instReg.DReg[0] = rt;
                                return LDRSBW64_POST;
                            }
                            case 0x04: {
                                ++instSetCounters[STRBFP64_POST];
                                instReg.SReg[4] = rt;
                                return STRBFP64_POST;
                            }
                            case 0x05: {
                                ++instSetCounters[LDRBFP64_POST];
                                instType.isLoad = 1;;
                                instReg.DReg[2] = rt;
                                return LDRBFP64_POST;
                            }
                            case 0x06:
                                ++instSetCounters[BigFpMemPostSt];
                                instType.isStore = 1;
                                instReg.SReg[4] = rt;
                                return BigFpMemPostSt;
                            case 0x07:
                                ++instSetCounters[BigFpMemPostLd];
                                instType.isLoad = 1;
                                instReg.DReg[2] = rt;
                                return BigFpMemPostLd;
                            case 0x08: {
                                ++instSetCounters[STRH64_POST];
                                instType.isStore = 1;
                                instReg.SReg[1] = rt;
                                return STRH64_POST;
                            }
                            case 0x09: {
                                ++instSetCounters[LDRH64_POST];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDRH64_POST;
                            }
                            case 0x0a: {
                                ++instSetCounters[LDRSHX64_POST];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDRSHX64_POST;
                            }
                            case 0x0b: {
                                ++instSetCounters[LDRSHW64_POST];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDRSHW64_POST;
                            }
                            case 0x0c: {
                                ++instSetCounters[STRHFP64_POST];
                                instType.isStore = 1;
                                instReg.SReg[4] = rt;
                                return STRHFP64_POST;
                            }
                            case 0x0d: {
                                ++instSetCounters[LDRHFP64_POST];
                                instType.isLoad = 1;
                                instReg.DReg[2] = rt;
                                return LDRHFP64_POST;
                            }
                            case 0x10: {
                                ++instSetCounters[STRW64_POST];
                                instType.isStore = 1;
                                instReg.SReg[1] = rt;
                                return STRW64_POST;
                            }
                            case 0x11: {
                                ++instSetCounters[LDRW64_POST];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDRW64_POST;
                            }
                            case 0x12: {
                                ++instSetCounters[LDRSW64_POST];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDRSW64_POST;
                            }
                            case 0x14: {
                                ++instSetCounters[STRSFP64_POST];
                                instType.isStore = 1;
                                instReg.SReg[4] = rt;
                                return STRSFP64_POST;
                            }
                            case 0x15: {
                                ++instSetCounters[LDRSFP64_POST];
                                instType.isLoad = 1;
                                instReg.DReg[2] = rt;
                                return LDRSFP64_POST;
                            }
                            case 0x18: {
                                ++instSetCounters[STRX64_POST];
                                instType.isStore = 1;
                                instReg.SReg[1] = rt;
                                return STRX64_POST;
                            }
                            case 0x19: {
                                ++instSetCounters[LDRX64_POST];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDRX64_POST;
                            }
                            case 0x1c: {
                                ++instSetCounters[STRDFP64_POST];
                                instType.isStore = 1;
                                instReg.SReg[4] = rt;
                                return STRDFP64_POST;
                            }
                            case 0x1d: {
                                ++instSetCounters[LDRDFP64_POST];
                                instType.isLoad = 1;
                                instReg.DReg[2] = rt;
                                return LDRDFP64_POST;
                            }
                            default: {
                                ++instSetCounters[Unknown64];
                                return Unknown64;

                            }
                        }
                    }
                    case 0x2: {
                        IntRegIndex rt = (IntRegIndex) (uint32_t) bits(machInst, 4, 0);
                        IntRegIndex rn = (IntRegIndex) (uint32_t) bits(machInst, 9, 5);
                        IntRegIndex rnsp = makeSP(rn);
                        instReg.SReg[0] = rn;
                        uint64_t imm = sext<9>(bits(machInst, 20, 12));
                        switch (switchVal) {
                            case 0x00: {
                                ++instSetCounters[STTRB64_IMM];
                                instType.isStore = 1;
                                instReg.SReg[1] = rt;
                                return STTRB64_IMM;
                            }
                            case 0x01: {
                                ++instSetCounters[LDTRB64_IMM];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDTRB64_IMM;
                            }
                            case 0x02: {
                                ++instSetCounters[LDTRSBX64_IMM];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
								return LDTRSBX64_IMM;
                            }
                            case 0x03: {
                                ++instSetCounters[LDTRSBW64_IMM];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDTRSBW64_IMM;
                            }
                            case 0x08: {
                                ++instSetCounters[STTRH64_IMM];
                                instType.isStore = 1;
                                instReg.SReg[1] = rt;
                                return STTRH64_IMM;
                            }
                            case 0x09: {
                                ++instSetCounters[LDTRH64_IMM];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDTRH64_IMM;
                            }
                            case 0x0a: {
                                ++instSetCounters[LDTRSHX64_IMM];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDTRSHX64_IMM;
                            }
                            case 0x0b: {
                                ++instSetCounters[LDTRSHW64_IMM];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDTRSHW64_IMM;
                            }
                            case 0x10: {
                                ++instSetCounters[STTRW64_IMM];
                                instType.isStore = 1;
                                instReg.SReg[1] = rt;
                                return STTRW64_IMM;
                            }
                            case 0x11: {
                                ++instSetCounters[LDTRW64_IMM];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDTRW64_IMM;
                            }
                            case 0x12: {
                                ++instSetCounters[LDTRSW64_IMM];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDTRSW64_IMM;
                            }
                            case 0x18: {
                                ++instSetCounters[STTRX64_IMM];
                                instType.isStore = 1;
                                instReg.SReg[1] = rt;
                                return STTRX64_IMM;
                            }
                            case 0x19: {
                                ++instSetCounters[LDTRX64_IMM];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDTRX64_IMM;
                            }
                            default: {
                                ++instSetCounters[Unknown64];
                                return Unknown64;

                            }
                        }
                    }
                    case 0x3: {
                        IntRegIndex rt = (IntRegIndex) (uint32_t) bits(machInst, 4, 0);
                        IntRegIndex rn = (IntRegIndex) (uint32_t) bits(machInst, 9, 5);
                        IntRegIndex rnsp = makeSP(rn);
                        instReg.SReg[0] = rn;
                        uint64_t imm = sext<9>(bits(machInst, 20, 12));
                        switch (switchVal) {
                            case 0x00: {
                                ++instSetCounters[STRB64_PRE];
                                instType.isStore = 1;
                                instReg.SReg[1] = rt;
                                return STRB64_PRE;
                            }
                            case 0x01: {
                                ++instSetCounters[LDRB64_PRE];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDRB64_PRE;
                            }
                            case 0x02: {
                                ++instSetCounters[LDRSBX64_PRE];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDRSBX64_PRE;
                            }
                            case 0x03: {
                                ++instSetCounters[LDRSBW64_PRE];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDRSBW64_PRE;
                            }
                            case 0x04: {
                                ++instSetCounters[STRBFP64_PRE];
                                instType.isStore = 1;
                                instReg.SReg[4] = rt;
                                return STRBFP64_PRE;
                            }
                            case 0x05: {
                                ++instSetCounters[LDRBFP64_PRE];
                                instType.isLoad = 1;
                                instReg.DReg[2] = rt;
                                return LDRBFP64_PRE;
                            }
                            case 0x06: {
                                ++instSetCounters[BigFpMemPreSt];
                                instType.isStore = 1;
                                instReg.SReg[4] = rt;
                                return BigFpMemPreSt;
                            }
                            case 0x07:
                                ++instSetCounters[BigFpMemPreLd];
                                instType.isLoad = 1;
                                instReg.DReg[2] = rt;
                                return BigFpMemPreLd;
                            case 0x08: {
                                ++instSetCounters[STRH64_PRE];
                                instType.isStore = 1;
                                instReg.SReg[1] = rt;
                                return STRH64_PRE;
                            }
                            case 0x09: {
                                ++instSetCounters[LDRH64_PRE];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDRH64_PRE;
                            }
                            case 0x0a: {
                                ++instSetCounters[LDRSHX64_PRE];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDRSHX64_PRE;
                            }
                            case 0x0b: {
                                ++instSetCounters[LDRSHW64_PRE];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDRSHW64_PRE;
                            }
                            case 0x0c: {
                                ++instSetCounters[STRHFP64_PRE];
                                instType.isStore = 1;
                                instReg.SReg[4] = rt;
                                return STRHFP64_PRE;
                            }
                            case 0x0d: {
                                ++instSetCounters[LDRHFP64_PRE];
                                instType.isLoad = 1;
                                instReg.DReg[2] = rt;
                                return LDRHFP64_PRE;
                            }
                            case 0x10: {
                                ++instSetCounters[STRW64_PRE];
                                instType.isStore = 1;
                                instReg.SReg[1] = rt;
                                return STRW64_PRE;
                            }
                            case 0x11: {
                                ++instSetCounters[LDRW64_PRE];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDRW64_PRE;
                            }
                            case 0x12: {
                                ++instSetCounters[LDRSW64_PRE];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDRSW64_PRE;
                            }
                            case 0x14: {
                                ++instSetCounters[STRSFP64_PRE];
                                instType.isStore = 1;
                                instReg.SReg[4] = rt;
                                return STRSFP64_PRE;
                            }
                            case 0x15: {
                                ++instSetCounters[LDRSFP64_PRE];
                                instType.isLoad = 1;
                                instReg.DReg[2] = rt;
                                return LDRSFP64_PRE;
                            }
                            case 0x18: {
                                ++instSetCounters[STRX64_PRE];
                                instType.isStore = 1;
                                instReg.SReg[1] = rt;
                                return STRX64_PRE;
                            }
                            case 0x19: {
                                ++instSetCounters[LDRX64_PRE];
                                instType.isLoad = 1;
                                instReg.DReg[0] = rt;
                                return LDRX64_PRE;
                            }
                            case 0x1c: {
                                ++instSetCounters[STRDFP64_PRE];
                                instType.isStore = 1;
                                instReg.SReg[4] = rt;
                                return STRDFP64_PRE;
                            }
                            case 0x1d: {
                                ++instSetCounters[LDRDFP64_PRE];
                                instType.isLoad = 1;
                                instReg.DReg[2] = rt;
                                return LDRDFP64_PRE;
                            }
                            default: {
                                ++instSetCounters[Unknown64];
                                return Unknown64;

                            }
                        }
                    }
                }
            }
        }
    }
    {
        ++instSetCounters[FailUnimplemented];
        return FailUnimplemented;
    }
}

inline Aarch64Set aarch64::decodeDataProcReg(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType) {
    uint8_t switchVal = (bits(machInst, 28) << 1) |
                        (bits(machInst, 24) << 0);
    switch (switchVal) {
        case 0x0: {
            uint8_t switchVal = (bits(machInst, 21) << 0) |
                                (bits(machInst, 30, 29) << 1);
            ArmShiftType type = (ArmShiftType) (uint8_t) bits(machInst, 23, 22);
            uint8_t imm6 = bits(machInst, 15, 10);
            bool sf = bits(machInst, 31);
            if (!sf && (imm6 & 0x20)) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            IntRegIndex rd = (IntRegIndex) (uint8_t) bits(machInst, 4, 0);
            IntRegIndex rdzr = makeZero(rd);
            IntRegIndex rn = (IntRegIndex) (uint8_t) bits(machInst, 9, 5);
            IntRegIndex rm = (IntRegIndex) (uint8_t) bits(machInst, 20, 16);
            /*  shen */
            instReg.SReg[0] = rn;
            instReg.SReg[1] = rm;
            instReg.DReg[0] = rd;
            switch (switchVal) {
                case 0x0: {
                    ++instSetCounters[AndXSReg];
                    return AndXSReg;

                }
                case 0x1: {
                    ++instSetCounters[BicXSReg];
                    return BicXSReg;

                }
                case 0x2: {
                    ++instSetCounters[OrrXSReg];
                    return OrrXSReg;

                }
                case 0x3: {
                    ++instSetCounters[OrnXSReg];
                    return OrnXSReg;

                }
                case 0x4: {
                    ++instSetCounters[EorXSReg];
                    return EorXSReg;

                }
                case 0x5: {
                    ++instSetCounters[EonXSReg];
                    return EonXSReg;

                }
                case 0x6: {
                    ++instSetCounters[AndXSRegCc];
					//instType.isCc = true;  // by shen 10/13
                    return AndXSRegCc;

                }
                case 0x7: {
                    ++instSetCounters[BicXSRegCc];
					//instType.isCc = true;  // by shen 10/13
                    return BicXSRegCc;

                }
            }
        }
        case 0x1: {
            uint8_t switchVal = bits(machInst, 30, 29);
            if (bits(machInst, 21) == 0) {
                ArmShiftType type =
                        (ArmShiftType) (uint8_t) bits(machInst, 23, 22);
                if (type == ROR) {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
                uint8_t imm6 = bits(machInst, 15, 10);
                if (!bits(machInst, 31) && bits(imm6, 5)) {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
                IntRegIndex rd = (IntRegIndex) (uint8_t) bits(machInst, 4, 0);
                IntRegIndex rdzr = makeZero(rd);
                IntRegIndex rn = (IntRegIndex) (uint8_t) bits(machInst, 9, 5);
                IntRegIndex rm = (IntRegIndex) (uint8_t) bits(machInst, 20, 16);
                /*  shen */
                instReg.SReg[0] = rn;
                instReg.SReg[1] = rm;
                instReg.DReg[0] = rd;
                switch (switchVal) {
                    case 0x0: {
                        ++instSetCounters[AddXSReg];
                        return AddXSReg;

                    }
                    case 0x1: {
                        ++instSetCounters[AddXSRegCc];
						//instType.isCc = true;  // by shen 10/13
                        return AddXSRegCc;

                    }
                    case 0x2: {
                        ++instSetCounters[SubXSReg];
                        return SubXSReg;

                    }
                    case 0x3: {
                        ++instSetCounters[SubXSRegCc];
						//instType.isCc = true;  // by shen 10/13
                        return SubXSRegCc;

                    }
                }
            } else {
                if (bits(machInst, 23, 22) != 0 || bits(machInst, 12, 10) > 0x4) {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
                ArmExtendType type =
                        (ArmExtendType) (uint8_t) bits(machInst, 15, 13);
                uint8_t imm3 = bits(machInst, 12, 10);
                IntRegIndex rd = (IntRegIndex) (uint8_t) bits(machInst, 4, 0);
                IntRegIndex rdsp = makeSP(rd);
                IntRegIndex rdzr = makeZero(rd);
                IntRegIndex rn = (IntRegIndex) (uint8_t) bits(machInst, 9, 5);
                IntRegIndex rnsp = makeSP(rn);
                IntRegIndex rm = (IntRegIndex) (uint8_t) bits(machInst, 20, 16);
                /*  shen */
                instReg.SReg[0] = rn;
                instReg.SReg[1] = rm;
                instReg.DReg[0] = rd;
                switch (switchVal) {
                    case 0x0: {
                        ++instSetCounters[AddXEReg];
                        return AddXEReg;

                    }
                    case 0x1: {
                        ++instSetCounters[AddXERegCc];
						//instType.isCc = true; // by shen 10,13
                        return AddXERegCc;

                    }
                    case 0x2: {
                        ++instSetCounters[SubXEReg];
                        return SubXEReg;

                    }
                    case 0x3: {
                        ++instSetCounters[SubXERegCc];
						//instType.isCc = true;  // by shen 10/13
                        return SubXERegCc;

                    }
                }
            }
        }
        case 0x2: {
            if (bits(machInst, 21) == 1) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            IntRegIndex rd = (IntRegIndex) (uint8_t) bits(machInst, 4, 0);
            IntRegIndex rdzr = makeZero(rd);
            IntRegIndex rn = (IntRegIndex) (uint8_t) bits(machInst, 9, 5);
            IntRegIndex rm = (IntRegIndex) (uint8_t) bits(machInst, 20, 16);

            switch (bits(machInst, 23, 22)) {
                case 0x0: {
                    /*  shen */
                    instReg.SReg[0] = rn;
                    instReg.SReg[1] = rm;
                    instReg.DReg[0] = rd;
                    if (bits(machInst, 15, 10)) {
                        ++instSetCounters[Unknown64];
                        return Unknown64;

                    }
                    uint8_t switchVal = bits(machInst, 30, 29);
                    switch (switchVal) {
                        case 0x0: {
                            ++instSetCounters[AdcXSReg];
                            return AdcXSReg;

                        }
                        case 0x1: {
                            ++instSetCounters[AdcXSRegCc];
							//instType.isCc = true;  // by shen 10/13
                            return AdcXSRegCc;

                        }
                        case 0x2: {
                            ++instSetCounters[SbcXSReg];
                            return SbcXSReg;

                        }
                        case 0x3: {
                            ++instSetCounters[SbcXSRegCc];
							//instType.isCc = true;
                            return SbcXSRegCc;

                        }
                    }
                }
                case 0x1: {
                    if ((bits(machInst, 4) == 1) ||
                        (bits(machInst, 10) == 1) ||
                        (bits(machInst, 29) == 0)) {
                        {
                            ++instSetCounters[Unknown64];
                            return Unknown64;

                        }
                    }
                    ConditionCode cond =
                            (ConditionCode) (uint8_t) bits(machInst, 15, 12);
					instType.isCc = true;
                    uint8_t flags = bits(machInst, 3, 0);
                    IntRegIndex rn = (IntRegIndex) (uint8_t) bits(machInst, 9, 5);
                    /*  shen */
                    instReg.SReg[0] = rn;
                    if (bits(machInst, 11) == 0) {
                        IntRegIndex rm =
                                (IntRegIndex) (uint8_t) bits(machInst, 20, 16);
                        /*  shen */
                        instReg.SReg[1] = rm;
                        if (bits(machInst, 30) == 0) {
                            {
                                ++instSetCounters[CcmnReg64];
                                return CcmnReg64;

                            }
                        } else {
                            {
                                ++instSetCounters[CcmpReg64];
                                return CcmpReg64;

                            }
                        }
                    } else {
                        uint8_t imm5 = bits(machInst, 20, 16);
                        /*  shen */
                        instReg.SReg[1] = rm;
                        instReg.SReg[0] = rn;
                        if (bits(machInst, 30) == 0) {
                            {
                                ++instSetCounters[CcmnImm64];
                                return CcmnImm64;

                            }
                        } else {
                            {
                                ++instSetCounters[CcmpImm64];
                                return CcmpImm64;

                            }
                        }
                    }
                }
                case 0x2: {
                    if (bits(machInst, 29) == 1 ||
                        bits(machInst, 11) == 1) {
                        {
                            ++instSetCounters[Unknown64];
                            return Unknown64;

                        }
                    }
                    uint8_t switchVal = (bits(machInst, 10) << 0) |
                                        (bits(machInst, 30) << 1);
                    IntRegIndex rd = (IntRegIndex) (uint8_t) bits(machInst, 4, 0);
                    IntRegIndex rdzr = makeZero(rd);
                    IntRegIndex rn = (IntRegIndex) (uint8_t) bits(machInst, 9, 5);
                    IntRegIndex rm = (IntRegIndex) (uint8_t) bits(machInst, 20, 16);
                    /* shen */
                    instReg.SReg[0] = rn;
                    instReg.SReg[1] = rm;
                    instReg.DReg[0] = rd;
                    ConditionCode cond =
                            (ConditionCode) (uint8_t) bits(machInst, 15, 12);
					instType.isCc = true;
                    switch (switchVal) {
                        case 0x0: {
                            ++instSetCounters[Csel64];
                            return Csel64;

                        }
                        case 0x1: {
                            ++instSetCounters[Csinc64];
                            return Csinc64;

                        }
                        case 0x2: {
                            ++instSetCounters[Csinv64];
                            return Csinv64;

                        }
                        case 0x3: {
                            ++instSetCounters[Csneg64];
                            return Csneg64;

                        }
                    }
                }
                case 0x3:
                    if (bits(machInst, 30) == 0) {
                        /* shen */
                        instReg.SReg[0] = rn;
                        instReg.SReg[1] = rm;
                        instReg.DReg[0] = rd;
                        if (bits(machInst, 29) != 0) {
                            ++instSetCounters[Unknown64];
                            return Unknown64;

                        }
                        uint8_t switchVal = bits(machInst, 15, 10);
                        switch (switchVal) {
                            case 0x2: {
                                ++instSetCounters[Udiv64];
                                return Udiv64;

                            }
                            case 0x3: {
                                ++instSetCounters[Sdiv64];
                                return Sdiv64;

                            }
                            case 0x8: {
                                ++instSetCounters[Lslv64];
                                return Lslv64;

                            }
                            case 0x9: {
                                ++instSetCounters[Lsrv64];
                                return Lsrv64;

                            }
                            case 0xa: {
                                ++instSetCounters[Asrv64];
                                return Asrv64;

                            }
                            case 0xb: {
                                ++instSetCounters[Rorv64];
                                return Rorv64;

                            }
                            default: {
                                ++instSetCounters[Unknown64];
                                return Unknown64;

                            }
                        }
                    } else {
                        /* shen */
                        instReg.SReg[0] = rn;
                        instReg.DReg[0] = rd;
                        if (bits(machInst, 20, 16) != 0 ||
                            bits(machInst, 29) != 0) {
                            {
                                ++instSetCounters[Unknown64];
                                return Unknown64;

                            }
                        }
                        uint8_t switchVal = bits(machInst, 15, 10);
                        switch (switchVal) {
                            case 0x0: {
                                ++instSetCounters[Rbit64];
                                return Rbit64;

                            }
                            case 0x1: {
                                ++instSetCounters[Rev1664];
                                return Rev1664;

                            }
                            case 0x2:
                                if (bits(machInst, 31) == 0) {
                                    ++instSetCounters[Rev64];
                                    return Rev64;

                                }
                                else {
                                    ++instSetCounters[Rev3264];
                                    return Rev3264;

                                }
                            case 0x3:
                                if (bits(machInst, 31) != 1) {
                                    ++instSetCounters[Unknown64];
                                    return Unknown64;

                                }
                                {
                                    ++instSetCounters[Rev64];
                                    return Rev64;

                                }
                            case 0x4: {
                                ++instSetCounters[Clz64];
                                return Clz64;

                            }
                            case 0x5: {
                                ++instSetCounters[Cls64];
                                return Cls64;

                            }
                        }
                    }
            }
        }
        case 0x3: {
            if (bits(machInst, 30, 29) != 0x0 ||
                (bits(machInst, 23, 21) != 0 && bits(machInst, 31) == 0)) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            IntRegIndex rd = (IntRegIndex) (uint8_t) bits(machInst, 4, 0);
            IntRegIndex rdzr = makeZero(rd);
            IntRegIndex rn = (IntRegIndex) (uint8_t) bits(machInst, 9, 5);
            IntRegIndex ra = (IntRegIndex) (uint8_t) bits(machInst, 14, 10);
            IntRegIndex rm = (IntRegIndex) (uint8_t) bits(machInst, 20, 16);
            /* shen   **/
            instReg.SReg[0] = rn;
            instReg.SReg[1] = rm;
            instReg.SReg[2] = ra;
            instReg.DReg[0] = rd;
            switch (bits(machInst, 23, 21)) {
                case 0x0:
                    if (bits(machInst, 15) == 0) {
                        ++instSetCounters[Madd64];
                        return Madd64;

                    }
                    else {
                        ++instSetCounters[Msub64];
                        return Msub64;

                    }
                case 0x1:
                    if (bits(machInst, 15) == 0) {
                        ++instSetCounters[Smaddl64];
                        return Smaddl64;

                    }
                    else {
                        ++instSetCounters[Smsubl64];
                        return Smsubl64;

                    }
                case 0x2:
                    if (bits(machInst, 15) != 0) {
                        ++instSetCounters[Unknown64];
                        return Unknown64;

                    }
                    {
                        ++instSetCounters[Smulh64];
                        return Smulh64;

                    }
                case 0x5:
                    if (bits(machInst, 15) == 0) {
                        ++instSetCounters[Umaddl64];
                        return Umaddl64;

                    }
                    else {
                        ++instSetCounters[Umsubl64];
                        return Umsubl64;

                    }
                case 0x6:
                    if (bits(machInst, 15) != 0) {
                        ++instSetCounters[Unknown64];
                        return Unknown64;

                    }
                    {
                        ++instSetCounters[Umulh64];
                        return Umulh64;

                    }
                default: {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
        }
    }
    {
        ++instSetCounters[FailUnimplemented];
        return FailUnimplemented;

    }
}

inline Aarch64Set aarch64::decodeFpAdvSIMD(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType) {
    if (bits(machInst, 28) == 0) {
        if (bits(machInst, 31) == 0) {
			return aarch64::decodeAdvSIMD(machInst, instSetCounters, instReg, instType);
        }
        else {
            ++instSetCounters[Unknown64];
            return Unknown64;

        }
    }
    else if (bits(machInst, 30) == 0) {
		return aarch64::decodeFp(machInst, instSetCounters, instReg, instType);
    }
    else if (bits(machInst, 31) == 0) {
		return aarch64::decodeAdvSIMDScalar(machInst, instSetCounters, instReg, instType);
    }
    else {
        ++instSetCounters[Unknown64];
        return Unknown64;

    }
}

inline Aarch64Set aarch64::decodeFp(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType) {
    if (bits(machInst, 24) == 1) {
        if (bits(machInst, 31) || bits(machInst, 29)) {
            ++instSetCounters[Unknown64];
            return Unknown64;

        }
        IntRegIndex rd = (IntRegIndex) (uint32_t) bits(machInst, 4, 0);
        IntRegIndex rn = (IntRegIndex) (uint32_t) bits(machInst, 9, 5);
        IntRegIndex rm = (IntRegIndex) (uint32_t) bits(machInst, 20, 16);
        IntRegIndex ra = (IntRegIndex) (uint32_t) bits(machInst, 14, 10);
        /* shen   **/
        instReg.SReg[3] = rn;
        instReg.SReg[4] = rm;
        instReg.SReg[5] = ra;
        instReg.DReg[2] = rd;
        uint8_t switchVal = (bits(machInst, 23, 21) << 1) |
                            (bits(machInst, 15) << 0);
        switch (switchVal) {
            case 0x0: // FMADD Sd = Sa + Sn*Sm
            {
                ++instSetCounters[FMAddS];
                return FMAddS;

            }
            case 0x1: // FMSUB Sd = Sa + (-Sn)*Sm
            {
                ++instSetCounters[FMSubS];
                return FMSubS;

            }
            case 0x2: // FNMADD Sd = (-Sa) + (-Sn)*Sm
            {
                ++instSetCounters[FNMAddS];
                return FNMAddS;

            }
            case 0x3: // FNMSUB Sd = (-Sa) + Sn*Sm
            {
                ++instSetCounters[FNMSubS];
                return FNMSubS;

            }
            case 0x4: // FMADD Dd = Da + Dn*Dm
            {
                ++instSetCounters[FMAddD];
                return FMAddD;

            }
            case 0x5: // FMSUB Dd = Da + (-Dn)*Dm
            {
                ++instSetCounters[FMSubD];
                return FMSubD;

            }
            case 0x6: // FNMADD Dd = (-Da) + (-Dn)*Dm
            {
                ++instSetCounters[FNMAddD];
                return FNMAddD;

            }
            case 0x7: // FNMSUB Dd = (-Da) + Dn*Dm
            {
                ++instSetCounters[FNMSubD];
                return FNMSubD;

            }
            default: {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
        }
    } 
    else if (bits(machInst, 21) == 0) {
        bool s = bits(machInst, 29);
        if (s) {
            ++instSetCounters[Unknown64];
            return Unknown64;

        }
        uint8_t switchVal = bits(machInst, 20, 16);
        uint8_t type = bits(machInst, 23, 22);
        uint8_t scale = bits(machInst, 15, 10);
        IntRegIndex rd = (IntRegIndex) (uint32_t) bits(machInst, 4, 0);
        IntRegIndex rn = (IntRegIndex) (uint32_t) bits(machInst, 9, 5);
        /* shen */
        instReg.SReg[3] = rn;
        instReg.DReg[2] = rd;
        if (bits(machInst, 18, 17) == 3 && scale != 0) {
            ++instSetCounters[Unknown64];
            return Unknown64;

        }
        // 30:24=0011110, 21=0
        switch (switchVal) {
            case 0x00: {
                ++instSetCounters[FailUnimplemented];
                return FailUnimplemented;

            }
            case 0x01: {
                ++instSetCounters[FailUnimplemented];
                return FailUnimplemented;

            }
            case 0x02:
                switch ((bits(machInst, 31) << 2) | type) {
                    case 0: // SCVTF Sd = convertFromInt(Wn/(2^fbits))
                    {
                        ++instSetCounters[FcvtSFixedFpSW];
                        return FcvtSFixedFpSW;

                    }
                    case 1: // SCVTF Dd = convertFromInt(Wn/(2^fbits))
                    {
                        ++instSetCounters[FcvtSFixedFpDW];
                        return FcvtSFixedFpDW;

                    }
                    case 4: // SCVTF Sd = convertFromInt(Xn/(2^fbits))
                    {
                        ++instSetCounters[FcvtSFixedFpSX];
                        return FcvtSFixedFpSX;

                    }
                    case 5: // SCVTF Dd = convertFromInt(Xn/(2^fbits))
                    {
                        ++instSetCounters[FcvtSFixedFpDX];
                        return FcvtSFixedFpDX;

                    }
                    default: {
                        ++instSetCounters[Unknown64];
                        return Unknown64;

                    }
                }
            case 0x03:
                switch ((bits(machInst, 31) << 2) | type) {
                    case 0: // UCVTF Sd = convertFromInt(Wn/(2^fbits))
                    {
                        ++instSetCounters[FcvtUFixedFpSW];
                        return FcvtUFixedFpSW;

                    }
                    case 1: // UCVTF Dd = convertFromInt(Wn/(2^fbits))
                    {
                        ++instSetCounters[FcvtUFixedFpDW];
                        return FcvtUFixedFpDW;

                    }
                    case 4: // UCVTF Sd = convertFromInt(Xn/(2^fbits))
                    {
                        ++instSetCounters[FcvtUFixedFpSX];
                        return FcvtUFixedFpSX;

                    }
                    case 5: // UCVTF Dd = convertFromInt(Xn/(2^fbits))
                    {
                        ++instSetCounters[FcvtUFixedFpDX];
                        return FcvtUFixedFpDX;

                    }
                    default: {
                        ++instSetCounters[Unknown64];
                        return Unknown64;

                    }
                }
            case 0x04: {
                ++instSetCounters[FailUnimplemented];
                return FailUnimplemented;

            }
            case 0x05: {
                ++instSetCounters[FailUnimplemented];
                return FailUnimplemented;

            }
            case 0x08: {
                ++instSetCounters[FailUnimplemented];
                return FailUnimplemented;

            }
            case 0x09: {
                ++instSetCounters[FailUnimplemented];
                return FailUnimplemented;

            }
            case 0x0e: {
                ++instSetCounters[FailUnimplemented];
                return FailUnimplemented;

            }
            case 0x0f: {
                ++instSetCounters[FailUnimplemented];
                return FailUnimplemented;

            }
            case 0x10: {
                ++instSetCounters[FailUnimplemented];
                return FailUnimplemented;

            }
            case 0x11: {
                ++instSetCounters[FailUnimplemented];
                return FailUnimplemented;

            }
            case 0x18:
                switch ((bits(machInst, 31) << 2) | type) {
                    case 0: // FCVTZS Wd = convertToIntExactTowardZero(Sn*(2^fbits))
                    {
                        ++instSetCounters[FcvtFpSFixedSW];
                        return FcvtFpSFixedSW;

                    }
                    case 1: // FCVTZS Wd = convertToIntExactTowardZero(Dn*(2^fbits))
                    {
                        ++instSetCounters[FcvtFpSFixedDW];
                        return FcvtFpSFixedDW;

                    }
                    case 4: // FCVTZS Xd = convertToIntExactTowardZero(Sn*(2^fbits))
                    {
                        ++instSetCounters[FcvtFpSFixedSX];
                        return FcvtFpSFixedSX;

                    }
                    case 5: // FCVTZS Xd = convertToIntExactTowardZero(Dn*(2^fbits))
                    {
                        ++instSetCounters[FcvtFpSFixedDX];
                        return FcvtFpSFixedDX;

                    }
                    default: {
                        ++instSetCounters[Unknown64];
                        return Unknown64;

                    }
                }
            case 0x19:
                switch ((bits(machInst, 31) << 2) | type) {
                    case 0: // FCVTZU Wd = convertToIntExactTowardZero(Sn*(2^fbits))
                    {
                        ++instSetCounters[FcvtFpUFixedSW];
                        return FcvtFpUFixedSW;

                    }
                    case 1: // FCVTZU Wd = convertToIntExactTowardZero(Dn*(2^fbits))
                    {
                        ++instSetCounters[FcvtFpUFixedDW];
                        return FcvtFpUFixedDW;

                    }
                    case 4: // FCVTZU Xd = convertToIntExactTowardZero(Sn*(2^fbits))
                    {
                        ++instSetCounters[FcvtFpUFixedSX];
                        return FcvtFpUFixedSX;

                    }
                    case 5: // FCVTZU Xd = convertToIntExactTowardZero(Dn*(2^fbits))
                    {
                        ++instSetCounters[FcvtFpUFixedDX];
                        return FcvtFpUFixedDX;

                    }
                    default: {
                        ++instSetCounters[Unknown64];
                        return Unknown64;

                    }
                }
        }
    } 
    else {
        // 30=0, 28:24=11110, 21=1
        uint8_t type = bits(machInst, 23, 22);
        uint8_t imm8 = bits(machInst, 20, 13);
        IntRegIndex rd = (IntRegIndex) (uint32_t) bits(machInst, 4, 0);
        IntRegIndex rn = (IntRegIndex) (uint32_t) bits(machInst, 9, 5);
        switch (bits(machInst, 11, 10)) {
            case 0x0:
                if (bits(machInst, 12) == 1) {
                    if (bits(machInst, 31) ||
                        bits(machInst, 29) ||
                        bits(machInst, 9, 5)) {
                        {
                            ++instSetCounters[Unknown64];
                            return Unknown64;

                        }
                    }
                    // 31:29=000, 28:24=11110, 21=1, 12:10=100
                    if (type == 0) {
                        // FMOV S[d] = imm8<7>:NOT(imm8<6>):Replicate(imm8<6>,5)
                        //             :imm8<5:0>:Zeros(19)
                        /* uint32_t imm = vfp_modified_imm(imm8, false); */
                        {            /* shen */
                            instReg.DReg[2] = rd;
                            ++instSetCounters[FmovImmS];
                            return FmovImmS;

                        }
                    } else if (type == 1) {
                        // FMOV D[d] = imm8<7>:NOT(imm8<6>):Replicate(imm8<6>,8)
                        //             :imm8<5:0>:Zeros(48)
                        /* uint64_t imm = vfp_modified_imm(imm8, true); */
                        {            /* shen */
                            instReg.DReg[2] = rd;
                            ++instSetCounters[FmovImmD];
                            return FmovImmD;

                        }
                    } else {
                        {
                            ++instSetCounters[Unknown64];
                            return Unknown64;

                        }
                    }
                } else if (bits(machInst, 13) == 1) {
                    if (bits(machInst, 31) ||
                        bits(machInst, 29) ||
                        bits(machInst, 15, 14) ||
                        bits(machInst, 23) ||
                        bits(machInst, 2, 0)) {
                        ++instSetCounters[Unknown64];
                        return Unknown64;

                    }
                    uint8_t switchVal = (bits(machInst, 4, 3) << 0) |
                                        (bits(machInst, 22) << 2);
                    IntRegIndex rm = (IntRegIndex) (uint32_t) bits(machInst, 20, 16);
                    /*  shen */
                    instReg.SReg[3] = rn;
                    // 28:23=000111100, 21=1, 15:10=001000, 2:0=000
                    switch (switchVal) {
                        case 0x0:
                            // FCMP flags = compareQuiet(Sn,Sm)
                        {
                            ++instSetCounters[FCmpRegS];
                            instReg.SReg[4] = rm;
							return FCmpRegS;
                        }
                        case 0x1:
                            // FCMP flags = compareQuiet(Sn,0.0)
                        {
                            ++instSetCounters[FCmpImmS];
                            return FCmpImmS;

                        }
                        case 0x2:
                            // FCMPE flags = compareSignaling(Sn,Sm)
                        {
                            ++instSetCounters[FCmpERegS];
                            instReg.SReg[4] = rm;
							return FCmpERegS;
                        }
                        case 0x3:
                            // FCMPE flags = compareSignaling(Sn,0.0)
                        {
                            ++instSetCounters[FCmpEImmS];
                            return FCmpEImmS;
                        }
                        case 0x4:
                            // FCMP flags = compareQuiet(Dn,Dm)
                        {
                            ++instSetCounters[FCmpRegD];
                            instReg.SReg[4] = rm;
							return FCmpRegD;
                        }
                        case 0x5:
                            // FCMP flags = compareQuiet(Dn,0.0)
                        {
                            ++instSetCounters[FCmpImmD];
                            return FCmpImmD;
                        }
                        case 0x6:
                            // FCMPE flags = compareSignaling(Dn,Dm)
                        {
                            ++instSetCounters[FCmpERegD];
                            instReg.SReg[4] = rm;
							return FCmpERegD;
                        }
                        case 0x7:
                            // FCMPE flags = compareSignaling(Dn,0.0)
                        {
                            ++instSetCounters[FCmpEImmD];
                            return FCmpEImmD;
                        }
                        default: {
                            ++instSetCounters[Unknown64];
                            return Unknown64;
                        }
                    }
                } else if (bits(machInst, 14) == 1) {
                    if (bits(machInst, 31) || bits(machInst, 29)) {
                        ++instSetCounters[Unknown64];
                        return Unknown64;
                    }
                    uint8_t opcode = bits(machInst, 20, 15);
                    // Bits 31:24=00011110, 21=1, 14:10=10000
                    /* shen */
                    instReg.DReg[2] = rd;
                    instReg.SReg[3] = rn;
                    switch (opcode) {
                        case 0x0:
                            if (type == 0)
                                // FMOV Sd = Sn
                            {
                                ++instSetCounters[FmovRegS];
                                return FmovRegS;

                            }
                            else if (type == 1)
                                // FMOV Dd = Dn
                            {
                                ++instSetCounters[FmovRegD];
                                return FmovRegD;

                            }
                            break;
                        case 0x1:
                            if (type == 0)
                                // FABS Sd = abs(Sn)
                            {
                                ++instSetCounters[FAbsS];
                                return FAbsS;

                            }
                            else if (type == 1)
                                // FABS Dd = abs(Dn)
                            {
                                ++instSetCounters[FAbsD];
                                return FAbsD;

                            }
                            break;
                        case 0x2:
                            if (type == 0)
                                // FNEG Sd = -Sn
                            {
                                ++instSetCounters[FNegS];
                                return FNegS;

                            }
                            else if (type == 1)
                                // FNEG Dd = -Dn
                            {
                                ++instSetCounters[FNegD];
                                return FNegD;

                            }
                            break;
                        case 0x3:
                            if (type == 0)
                                // FSQRT Sd = sqrt(Sn)
                            {
                                ++instSetCounters[FSqrtS];
                                return FSqrtS;

                            }
                            else if (type == 1)
                                // FSQRT Dd = sqrt(Dn)
                            {
                                ++instSetCounters[FSqrtD];
                                return FSqrtD;

                            }
                            break;
                        case 0x4:
                            if (type == 1)
                                // FCVT Sd = convertFormat(Dn)
                            {
                                ++instSetCounters[FcvtFpDFpS];
                                return FcvtFpDFpS;

                            }
                            else if (type == 3)
                                // FCVT Sd = convertFormat(Hn)
                            {
                                ++instSetCounters[FcvtFpHFpS];
                                return FcvtFpHFpS;

                            }
                            break;
                        case 0x5:
                            if (type == 0)
                                // FCVT Dd = convertFormat(Sn)
                            {
                                ++instSetCounters[FCvtFpSFpD];
                                return FCvtFpSFpD;

                            }
                            else if (type == 3)
                                // FCVT Dd = convertFormat(Hn)
                            {
                                ++instSetCounters[FcvtFpHFpD];
                                return FcvtFpHFpD;

                            }
                            break;
                        case 0x7:
                            if (type == 0)
                                // FCVT Hd = convertFormat(Sn)
                            {
                                ++instSetCounters[FcvtFpSFpH];
                                return FcvtFpSFpH;

                            }
                            else if (type == 1)
                                // FCVT Hd = convertFormat(Dn)
                            {
                                ++instSetCounters[FcvtFpDFpH];
                                return FcvtFpDFpH;

                            }
                            break;
                        case 0x8:
                            if (type == 0) // FRINTN Sd = roundToIntegralTiesToEven(Sn)
                            {
                                ++instSetCounters[FRIntNS];
                                return FRIntNS;

                            }
                            else if (type == 1) // FRINTN Dd = roundToIntegralTiesToEven(Dn)
                            {
                                ++instSetCounters[FRIntND];
                                return FRIntND;

                            }
                            break;
                        case 0x9:
                            if (type == 0) // FRINTP Sd = roundToIntegralTowardPlusInf(Sn)
                            {
                                ++instSetCounters[FRIntPS];
                                return FRIntPS;

                            }
                            else if (type == 1) // FRINTP Dd = roundToIntegralTowardPlusInf(Dn)
                            {
                                ++instSetCounters[FRIntPD];
                                return FRIntPD;

                            }
                            break;
                        case 0xa:
                            if (type == 0) // FRINTM Sd = roundToIntegralTowardMinusInf(Sn)
                            {
                                ++instSetCounters[FRIntMS];
                                return FRIntMS;

                            }
                            else if (type == 1) // FRINTM Dd = roundToIntegralTowardMinusInf(Dn)
                            {
                                ++instSetCounters[FRIntMD];
                                return FRIntMD;

                            }
                            break;
                        case 0xb:
                            if (type == 0) // FRINTZ Sd = roundToIntegralTowardZero(Sn)
                            {
                                ++instSetCounters[FRIntZS];
                                return FRIntZS;

                            }
                            else if (type == 1) // FRINTZ Dd = roundToIntegralTowardZero(Dn)
                            {
                                ++instSetCounters[FRIntZD];
                                return FRIntZD;

                            }
                            break;
                        case 0xc:
                            if (type == 0) // FRINTA Sd = roundToIntegralTiesToAway(Sn)
                            {
                                ++instSetCounters[FRIntAS];
                                return FRIntAS;

                            }
                            else if (type == 1) // FRINTA Dd = roundToIntegralTiesToAway(Dn)
                            {
                                ++instSetCounters[FRIntAD];
                                return FRIntAD;

                            }
                            break;
                        case 0xe:
                            if (type == 0) // FRINTX Sd = roundToIntegralExact(Sn)
                            {
                                ++instSetCounters[FRIntXS];
                                return FRIntXS;

                            }
                            else if (type == 1) // FRINTX Dd = roundToIntegralExact(Dn)
                            {
                                ++instSetCounters[FRIntXD];
                                return FRIntXD;

                            }
                            break;
                        case 0xf:
                            if (type == 0) // FRINTI Sd = roundToIntegral(Sn)
                            {
                                ++instSetCounters[FRIntIS];
                                return FRIntIS;

                            }
                            else if (type == 1) // FRINTI Dd = roundToIntegral(Dn)
                            {
                                ++instSetCounters[FRIntID];
                                return FRIntID;

                            }
                            break;
                        default: {
                            ++instSetCounters[Unknown64];
                            return Unknown64;

                        }
                    }
                    {
                        ++instSetCounters[Unknown64];
                        return Unknown64;

                    }
                } else if (bits(machInst, 15) == 1) {
                    {
                        ++instSetCounters[Unknown64];
                        return Unknown64;

                    }
                } else {
                    if (bits(machInst, 29)) {
                        ++instSetCounters[Unknown64];
                        return Unknown64;

                    }
                    uint8_t rmode = bits(machInst, 20, 19);
                    uint8_t switchVal1 = bits(machInst, 18, 16);
                    uint8_t switchVal2 = (type << 1) | bits(machInst, 31);
                    // 30:24=0011110, 21=1, 15:10=000000
                    /*  shen   **/
                    instReg.SReg[3] = rn;
                    instReg.DReg[2] = rd;
                    switch (switchVal1) {
                        case 0x0:
                            switch ((switchVal2 << 2) | rmode) {
                                case 0x0: //FCVTNS Wd = convertToIntExactTiesToEven(Sn)
                                {
                                    ++instSetCounters[FcvtFpSIntWSN];
                                    return FcvtFpSIntWSN;

                                }
                                case 0x1: //FCVTPS Wd = convertToIntExactTowardPlusInf(Sn)
                                {
                                    ++instSetCounters[FcvtFpSIntWSP];
                                    return FcvtFpSIntWSP;

                                }
                                case 0x2: //FCVTMS Wd = convertToIntExactTowardMinusInf(Sn)
                                {
                                    ++instSetCounters[FcvtFpSIntWSM];
                                    return FcvtFpSIntWSM;

                                }
                                case 0x3: //FCVTZS Wd = convertToIntExactTowardZero(Sn)
                                {
                                    ++instSetCounters[FcvtFpSIntWSZ];
                                    return FcvtFpSIntWSZ;

                                }
                                case 0x4: //FCVTNS Xd = convertToIntExactTiesToEven(Sn)
                                {
                                    ++instSetCounters[FcvtFpSIntXSN];
                                    return FcvtFpSIntXSN;

                                }
                                case 0x5: //FCVTPS Xd = convertToIntExactTowardPlusInf(Sn)
                                {
                                    ++instSetCounters[FcvtFpSIntXSP];
                                    return FcvtFpSIntXSP;

                                }
                                case 0x6: //FCVTMS Xd = convertToIntExactTowardMinusInf(Sn)
                                {
                                    ++instSetCounters[FcvtFpSIntXSM];
                                    return FcvtFpSIntXSM;

                                }
                                case 0x7: //FCVTZS Xd = convertToIntExactTowardZero(Sn)
                                {
                                    ++instSetCounters[FcvtFpSIntXSZ];
                                    return FcvtFpSIntXSZ;

                                }
                                case 0x8: //FCVTNS Wd = convertToIntExactTiesToEven(Dn)
                                {
                                    ++instSetCounters[FcvtFpSIntWDN];
                                    return FcvtFpSIntWDN;

                                }
                                case 0x9: //FCVTPS Wd = convertToIntExactTowardPlusInf(Dn)
                                {
                                    ++instSetCounters[FcvtFpSIntWDP];
                                    return FcvtFpSIntWDP;

                                }
                                case 0xA: //FCVTMS Wd = convertToIntExactTowardMinusInf(Dn)
                                {
                                    ++instSetCounters[FcvtFpSIntWDM];
                                    return FcvtFpSIntWDM;

                                }
                                case 0xB: //FCVTZS Wd = convertToIntExactTowardZero(Dn)
                                {
                                    ++instSetCounters[FcvtFpSIntWDZ];
                                    return FcvtFpSIntWDZ;

                                }
                                case 0xC: //FCVTNS Xd = convertToIntExactTiesToEven(Dn)
                                {
                                    ++instSetCounters[FcvtFpSIntXDN];
                                    return FcvtFpSIntXDN;

                                }
                                case 0xD: //FCVTPS Xd = convertToIntExactTowardPlusInf(Dn)
                                {
                                    ++instSetCounters[FcvtFpSIntXDP];
                                    return FcvtFpSIntXDP;

                                }
                                case 0xE: //FCVTMS Xd = convertToIntExactTowardMinusInf(Dn)
                                {
                                    ++instSetCounters[FcvtFpSIntXDM];
                                    return FcvtFpSIntXDM;

                                }
                                case 0xF: //FCVTZS Xd = convertToIntExactTowardZero(Dn)
                                {
                                    ++instSetCounters[FcvtFpSIntXDZ];
                                    return FcvtFpSIntXDZ;

                                }
                                default: {
                                    ++instSetCounters[Unknown64];
                                    return Unknown64;

                                }
                            }
                        case 0x1:
                            switch ((switchVal2 << 2) | rmode) {
                                case 0x0: //FCVTNU Wd = convertToIntExactTiesToEven(Sn)
                                {
                                    ++instSetCounters[FcvtFpUIntWSN];
                                    return FcvtFpUIntWSN;

                                }
                                case 0x1: //FCVTPU Wd = convertToIntExactTowardPlusInf(Sn)
                                {
                                    ++instSetCounters[FcvtFpUIntWSP];
                                    return FcvtFpUIntWSP;

                                }
                                case 0x2: //FCVTMU Wd = convertToIntExactTowardMinusInf(Sn)
                                {
                                    ++instSetCounters[FcvtFpUIntWSM];
                                    return FcvtFpUIntWSM;

                                }
                                case 0x3: //FCVTZU Wd = convertToIntExactTowardZero(Sn)
                                {
                                    ++instSetCounters[FcvtFpUIntWSZ];
                                    return FcvtFpUIntWSZ;

                                }
                                case 0x4: //FCVTNU Xd = convertToIntExactTiesToEven(Sn)
                                {
                                    ++instSetCounters[FcvtFpUIntXSN];
                                    return FcvtFpUIntXSN;

                                }
                                case 0x5: //FCVTPU Xd = convertToIntExactTowardPlusInf(Sn)
                                {
                                    ++instSetCounters[FcvtFpUIntXSP];
                                    return FcvtFpUIntXSP;

                                }
                                case 0x6: //FCVTMU Xd = convertToIntExactTowardMinusInf(Sn)
                                {
                                    ++instSetCounters[FcvtFpUIntXSM];
                                    return FcvtFpUIntXSM;

                                }
                                case 0x7: //FCVTZU Xd = convertToIntExactTowardZero(Sn)
                                {
                                    ++instSetCounters[FcvtFpUIntXSZ];
                                    return FcvtFpUIntXSZ;

                                }
                                case 0x8: //FCVTNU Wd = convertToIntExactTiesToEven(Dn)
                                {
                                    ++instSetCounters[FcvtFpUIntWDN];
                                    return FcvtFpUIntWDN;

                                }
                                case 0x9: //FCVTPU Wd = convertToIntExactTowardPlusInf(Dn)
                                {
                                    ++instSetCounters[FcvtFpUIntWDP];
                                    return FcvtFpUIntWDP;

                                }
                                case 0xA: //FCVTMU Wd = convertToIntExactTowardMinusInf(Dn)
                                {
                                    ++instSetCounters[FcvtFpUIntWDM];
                                    return FcvtFpUIntWDM;

                                }
                                case 0xB: //FCVTZU Wd = convertToIntExactTowardZero(Dn)
                                {
                                    ++instSetCounters[FcvtFpUIntWDZ];
                                    return FcvtFpUIntWDZ;

                                }
                                case 0xC: //FCVTNU Xd = convertToIntExactTiesToEven(Dn)
                                {
                                    ++instSetCounters[FcvtFpUIntXDN];
                                    return FcvtFpUIntXDN;

                                }
                                case 0xD: //FCVTPU Xd = convertToIntExactTowardPlusInf(Dn)
                                {
                                    ++instSetCounters[FcvtFpUIntXDP];
                                    return FcvtFpUIntXDP;

                                }
                                case 0xE: //FCVTMU Xd = convertToIntExactTowardMinusInf(Dn)
                                {
                                    ++instSetCounters[FcvtFpUIntXDM];
                                    return FcvtFpUIntXDM;

                                }
                                case 0xF: //FCVTZU Xd = convertToIntExactTowardZero(Dn)
                                {
                                    ++instSetCounters[FcvtFpUIntXDZ];
                                    return FcvtFpUIntXDZ;

                                }
                                default: {
                                    ++instSetCounters[Unknown64];
                                    return Unknown64;

                                }
                            }
                        case 0x2:
                            if (rmode != 0) {
                                ++instSetCounters[Unknown64];
                                return Unknown64;

                            }
                            switch (switchVal2) {
                                case 0: // SCVTF Sd = convertFromInt(Wn)
                                {
                                    ++instSetCounters[FcvtWSIntFpS];
                                    return FcvtWSIntFpS;

                                }
                                case 1: // SCVTF Sd = convertFromInt(Xn)
                                {
                                    ++instSetCounters[FcvtXSIntFpS];
                                    return FcvtXSIntFpS;

                                }
                                case 2: // SCVTF Dd = convertFromInt(Wn)
                                {
                                    ++instSetCounters[FcvtWSIntFpD];
                                    return FcvtWSIntFpD;

                                }
                                case 3: // SCVTF Dd = convertFromInt(Xn)
                                {
                                    ++instSetCounters[FcvtXSIntFpD];
                                    return FcvtXSIntFpD;

                                }
                                default: {
                                    ++instSetCounters[Unknown64];
                                    return Unknown64;

                                }
                            }
                        case 0x3:
                            switch (switchVal2) {
                                case 0: // UCVTF Sd = convertFromInt(Wn)
                                {
                                    ++instSetCounters[FcvtWUIntFpS];
                                    return FcvtWUIntFpS;

                                }
                                case 1: // UCVTF Sd = convertFromInt(Xn)
                                {
                                    ++instSetCounters[FcvtXUIntFpS];
                                    return FcvtXUIntFpS;

                                }
                                case 2: // UCVTF Dd = convertFromInt(Wn)
                                {
                                    ++instSetCounters[FcvtWUIntFpD];
                                    return FcvtWUIntFpD;

                                }
                                case 3: // UCVTF Dd = convertFromInt(Xn)
                                {
                                    ++instSetCounters[FcvtXUIntFpD];
                                    return FcvtXUIntFpD;

                                }
                                default: {
                                    ++instSetCounters[Unknown64];
                                    return Unknown64;

                                }
                            }
                        case 0x4:
                            if (rmode != 0) {
                                ++instSetCounters[Unknown64];
                                return Unknown64;

                            }
                            switch (switchVal2) {
                                case 0: // FCVTAS Wd = convertToIntExactTiesToAway(Sn)
                                {
                                    ++instSetCounters[FcvtFpSIntWSA];
                                    return FcvtFpSIntWSA;

                                }
                                case 1: // FCVTAS Xd = convertToIntExactTiesToAway(Sn)
                                {
                                    ++instSetCounters[FcvtFpSIntXSA];
                                    return FcvtFpSIntXSA;

                                }
                                case 2: // FCVTAS Wd = convertToIntExactTiesToAway(Dn)
                                {
                                    ++instSetCounters[FcvtFpSIntWDA];
                                    return FcvtFpSIntWDA;

                                }
                                case 3: // FCVTAS Wd = convertToIntExactTiesToAway(Dn)
                                {
                                    ++instSetCounters[FcvtFpSIntXDA];
                                    return FcvtFpSIntXDA;

                                }
                                default: {
                                    ++instSetCounters[Unknown64];
                                    return Unknown64;

                                }
                            }
                        case 0x5:
                            switch (switchVal2) {
                                case 0: // FCVTAU Wd = convertToIntExactTiesToAway(Sn)
                                {
                                    ++instSetCounters[FcvtFpUIntWSA];
                                    return FcvtFpUIntWSA;

                                }
                                case 1: // FCVTAU Xd = convertToIntExactTiesToAway(Sn)
                                {
                                    ++instSetCounters[FcvtFpUIntXSA];
                                    return FcvtFpUIntXSA;

                                }
                                case 2: // FCVTAU Wd = convertToIntExactTiesToAway(Dn)
                                {
                                    ++instSetCounters[FcvtFpUIntWDA];
                                    return FcvtFpUIntWDA;

                                }
                                case 3: // FCVTAU Xd = convertToIntExactTiesToAway(Dn)
                                {
                                    ++instSetCounters[FcvtFpUIntXDA];
                                    return FcvtFpUIntXDA;

                                }
                                default: {
                                    ++instSetCounters[Unknown64];
                                    return Unknown64;

                                }
                            }
                        case 0x06:
                            switch (switchVal2) {
                                case 0: // FMOV Wd = Sn
                                    if (rmode != 0) {
                                        ++instSetCounters[Unknown64];
                                        return Unknown64;

                                    }
                                    {
                                        ++instSetCounters[FmovRegCoreW];
                                        return FmovRegCoreW;

                                    }
                                case 3: // FMOV Xd = Dn
                                    if (rmode != 0) {
                                        ++instSetCounters[Unknown64];
                                        return Unknown64;

                                    }
                                    {
                                        ++instSetCounters[FmovRegCoreX];
                                        return FmovRegCoreX;

                                    }
                                case 5: // FMOV Xd = Vn<127:64>
                                    if (rmode != 1) {
                                        ++instSetCounters[Unknown64];
                                        return Unknown64;

                                    }
                                    {
                                        ++instSetCounters[FmovURegCoreX];
                                        return FmovURegCoreX;

                                    }
                                default: {
                                    ++instSetCounters[Unknown64];
                                    return Unknown64;

                                }
                            }
                            break;
                        case 0x07:
                            switch (switchVal2) {
                                case 0: // FMOV Sd = Wn
                                    if (rmode != 0) {
                                        ++instSetCounters[Unknown64];
                                        return Unknown64;

                                    }
                                    {
                                        ++instSetCounters[FmovCoreRegW];
                                        return FmovCoreRegW;

                                    }
                                case 3: // FMOV Xd = Dn
                                    if (rmode != 0) {
                                        ++instSetCounters[Unknown64];
                                        return Unknown64;

                                    }
                                    {
                                        ++instSetCounters[FmovCoreRegX];
                                        return FmovCoreRegX;

                                    }
                                case 5: // FMOV Xd = Vn<127:64>
                                    if (rmode != 1) {
                                        ++instSetCounters[Unknown64];
                                        return Unknown64;

                                    }
                                    {
                                        ++instSetCounters[FmovUCoreRegX];
                                        return FmovUCoreRegX;

                                    }
                                default: {
                                    ++instSetCounters[Unknown64];
                                    return Unknown64;

                                }
                            }
                            break;
                        default: // Warning! missing cases in switch statement above, that still need to be added
                        {
                            ++instSetCounters[Unknown64];
                            return Unknown64;

                        }
                    }
                }
            case 0x1: {
                if (bits(machInst, 31) ||
                    bits(machInst, 29) ||
                    bits(machInst, 23)) {
                    {
                        ++instSetCounters[Unknown64];
                        return Unknown64;

                    }
                }
                IntRegIndex rm = (IntRegIndex) (uint32_t) bits(machInst, 20, 16);
                IntRegIndex rn = (IntRegIndex) (uint32_t) bits(machInst, 9, 5);
                /* shen */
                instReg.SReg[3] = rn;
                instReg.SReg[4] = rm;
                uint8_t imm = (IntRegIndex) (uint32_t) bits(machInst, 3, 0);
                ConditionCode cond =
                        (ConditionCode) (uint8_t) (bits(machInst, 15, 12));
				instType.isCc = true;
                uint8_t switchVal = (bits(machInst, 4) << 0) |
                                    (bits(machInst, 22) << 1);
                // 31:23=000111100, 21=1, 11:10=01
                switch (switchVal) {
                    case 0x0:
                        // FCCMP flags = if cond the compareQuiet(Sn,Sm) else #nzcv
                    {
                        ++instSetCounters[FCCmpRegS];
                        return FCCmpRegS;

                    }
                    case 0x1:
                        // FCCMP flags = if cond then compareSignaling(Sn,Sm)
                        //               else #nzcv
                    {
                        ++instSetCounters[FCCmpERegS];
                        return FCCmpERegS;

                    }
                    case 0x2:
                        // FCCMP flags = if cond then compareQuiet(Dn,Dm) else #nzcv
                    {
                        ++instSetCounters[FCCmpRegD];
                        return FCCmpRegD;

                    }
                    case 0x3:
                        // FCCMP flags = if cond then compareSignaling(Dn,Dm)
                        //               else #nzcv
                    {
                        ++instSetCounters[FCCmpERegD];
                        return FCCmpERegD;

                    }
                    default: {
                        ++instSetCounters[Unknown64];
                        return Unknown64;

                    }
                }
            }
            case 0x2: {
                if (bits(machInst, 31) ||
                    bits(machInst, 29) ||
                    bits(machInst, 23)) {
                    {
                        ++instSetCounters[Unknown64];
                        return Unknown64;

                    }
                }
                IntRegIndex rd = (IntRegIndex) (uint32_t) bits(machInst, 4, 0);
                IntRegIndex rn = (IntRegIndex) (uint32_t) bits(machInst, 9, 5);
                IntRegIndex rm = (IntRegIndex) (uint32_t) bits(machInst, 20, 16);
                /* shen */
                instReg.DReg[2] = rd;
                instReg.SReg[3] = rn;
                instReg.SReg[4] = rm;
                uint8_t switchVal = (bits(machInst, 15, 12) << 0) |
                                    (bits(machInst, 22) << 4);
                switch (switchVal) {
                    case 0x00: // FMUL Sd = Sn * Sm
                    {
                        ++instSetCounters[FMulS];
                        return FMulS;

                    }
                    case 0x10: // FMUL Dd = Dn * Dm
                    {
                        ++instSetCounters[FMulD];
                        return FMulD;

                    }
                    case 0x01: // FDIV Sd = Sn / Sm
                    {
                        ++instSetCounters[FDivS];
                        return FDivS;

                    }
                    case 0x11: // FDIV Dd = Dn / Dm
                    {
                        ++instSetCounters[FDivD];
                        return FDivD;

                    }
                    case 0x02: // FADD Sd = Sn + Sm
                    {
                        ++instSetCounters[FAddS];
                        return FAddS;

                    }
                    case 0x12: // FADD Dd = Dn + Dm
                    {
                        ++instSetCounters[FAddD];
                        return FAddD;

                    }
                    case 0x03: // FSUB Sd = Sn - Sm
                    {
                        ++instSetCounters[FSubS];
                        return FSubS;

                    }
                    case 0x13: // FSUB Dd = Dn - Dm
                    {
                        ++instSetCounters[FSubD];
                        return FSubD;

                    }
                    case 0x04: // FMAX Sd = max(Sn, Sm)
                    {
                        ++instSetCounters[FMaxS];
                        return FMaxS;

                    }
                    case 0x14: // FMAX Dd = max(Dn, Dm)
                    {
                        ++instSetCounters[FMaxD];
                        return FMaxD;

                    }
                    case 0x05: // FMIN Sd = min(Sn, Sm)
                    {
                        ++instSetCounters[FMinS];
                        return FMinS;

                    }
                    case 0x15: // FMIN Dd = min(Dn, Dm)
                    {
                        ++instSetCounters[FMinD];
                        return FMinD;

                    }
                    case 0x06: // FMAXNM Sd = maxNum(Sn, Sm)
                    {
                        ++instSetCounters[FMaxNMS];
                        return FMaxNMS;

                    }
                    case 0x16: // FMAXNM Dd = maxNum(Dn, Dm)
                    {
                        ++instSetCounters[FMaxNMD];
                        return FMaxNMD;

                    }
                    case 0x07: // FMINNM Sd = minNum(Sn, Sm)
                    {
                        ++instSetCounters[FMinNMS];
                        return FMinNMS;

                    }
                    case 0x17: // FMINNM Dd = minNum(Dn, Dm)
                    {
                        ++instSetCounters[FMinNMD];
                        return FMinNMD;

                    }
                    case 0x08: // FNMUL Sd = -(Sn * Sm)
                    {
                        ++instSetCounters[FNMulS];
                        return FNMulS;

                    }
                    case 0x18: // FNMUL Dd = -(Dn * Dm)
                    {
                        ++instSetCounters[FNMulD];
                        return FNMulD;

                    }
                    default: {
                        ++instSetCounters[Unknown64];
                        return Unknown64;

                    }
                }
            }
            case 0x3: {
                if (bits(machInst, 31) || bits(machInst, 29)) {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
                uint8_t type = bits(machInst, 23, 22);
                IntRegIndex rd = (IntRegIndex) (uint32_t) bits(machInst, 4, 0);
                IntRegIndex rn = (IntRegIndex) (uint32_t) bits(machInst, 9, 5);
                IntRegIndex rm = (IntRegIndex) (uint32_t) bits(machInst, 20, 16);
                /* shen */
                instReg.DReg[2] = rd;
                instReg.SReg[3] = rn;
                instReg.SReg[4] = rm;
                ConditionCode cond =
                        (ConditionCode) (uint8_t) (bits(machInst, 15, 12));
				instType.isCc = true;
                if (type == 0) // FCSEL Sd = if cond then Sn else Sm
                {
                    ++instSetCounters[FCSelS];
                    return FCSelS;

                }
                else if (type == 1) // FCSEL Dd = if cond then Dn else Dm
                {
                    ++instSetCounters[FCSelD];
                    return FCSelD;

                }
                else {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
        }
    }
    ++instSetCounters[FailUnimplemented];
    return FailUnimplemented;

}

inline Aarch64Set aarch64::decodeAdvSIMD(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType) {
    if (bits(machInst, 24) == 1) {
        if (bits(machInst, 10) == 0) {
			return aarch64::decodeNeonIndexedElem(machInst, instSetCounters, instReg);
        }
        else if (bits(machInst, 23) == 1) {
            ++instSetCounters[Unknown64];
            return Unknown64;

        }
        else {
            if (bits(machInst, 22, 19)) {
				return aarch64::decodeNeonShiftByImm(machInst, instSetCounters, instReg);
            }
            else {
				return aarch64::decodeNeonModImm(machInst, instSetCounters, instReg);
            }
        }
    }
    else if (bits(machInst, 21) == 1) {
        if (bits(machInst, 10) == 1) {
			return aarch64::decodeNeon3Same(machInst, instSetCounters, instReg);
        }
        else if (bits(machInst, 11) == 0) {
			return aarch64::decodeNeon3Diff(machInst, instSetCounters, instReg);
        }
        else if (bits(machInst, 20, 17) == 0x0) {
			instType.isMISC = true;
			return aarch64::decodeNeon2RegMisc(machInst, instSetCounters, instReg);
        }
        else if (bits(machInst, 20, 17) == 0x8) {
			return aarch64::decodeNeonAcrossLanes(machInst, instSetCounters, instReg);
        }
        else {
            ++instSetCounters[Unknown64];
            return Unknown64;

        }
    }
    else if (bits(machInst, 24) ||
             bits(machInst, 21) ||
             bits(machInst, 15)) {
        ++instSetCounters[Unknown64];
        return Unknown64;

    }
    else if (bits(machInst, 10) == 1) {         //[modified by pz]
        if (bits(machInst, 23, 22) == 0)
            return aarch64::decodeNeonCopy(machInst, instSetCounters, instReg);
 //     else if ((bits(machInst, 22) == 1) && (bits(machInst, 14) == 0))
 //         return aarch64::decodeNeon3SameFP(machInst, instSetCounters, instReg);
        else  {
            ++instSetCounters[Unknown64];
            return Unknown64;
              }
    }
    else if (bits(machInst, 29) == 1) {
		return aarch64::decodeNeonExt(machInst, instSetCounters, instReg);
    }
    else if (bits(machInst, 11) == 1) {
		return aarch64::decodeNeonZipUzpTrn(machInst, instSetCounters, instReg);
    }
    else if (bits(machInst, 23, 22) == 0x0) {
		return aarch64::decodeNeonTblTbx(machInst, instSetCounters, instReg);
    }
    else {
        ++instSetCounters[Unknown64];
        return Unknown64;

    }
    ++instSetCounters[FailUnimplemented];
    return FailUnimplemented;

}

inline Aarch64Set aarch64::decodeAdvSIMDScalar(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg, InstType &instType) {
    if (bits(machInst, 24) == 1) {
        if (bits(machInst, 10) == 0) {
			return aarch64::decodeNeonScIndexedElem(machInst, instSetCounters, instReg);
        }
        else if (bits(machInst, 23) == 0) {
			return aarch64::decodeNeonScShiftByImm(machInst, instSetCounters, instReg);
        }
    }
    else if (bits(machInst, 21) == 1) {
        if (bits(machInst, 10) == 1) {
			return aarch64::decodeNeonSc3Same(machInst, instSetCounters, instReg);
        }
        else if (bits(machInst, 11) == 0) {
			return aarch64::decodeNeonSc3Diff(machInst, instSetCounters, instReg);
        }
        else if (bits(machInst, 20, 17) == 0x0) {
			instType.isMISC = true;
			return aarch64::decodeNeonSc2RegMisc(machInst, instSetCounters, instReg);
        }
        else if (bits(machInst, 20, 17) == 0x8) {
			return aarch64::decodeNeonScPwise(machInst, instSetCounters, instReg);
        }
        else {
            ++instSetCounters[Unknown64];
            return Unknown64;

        }
    }
    else if (bits(machInst, 23, 22) == 0 &&
             bits(machInst, 15) == 0 &&
             bits(machInst, 10) == 1) {
		return aarch64::decodeNeonScCopy(machInst, instSetCounters, instReg);
    }
    else {
        ++instSetCounters[Unknown64];
        return Unknown64;

    }
    ++instSetCounters[FailUnimplemented];
    return FailUnimplemented;

}

/* Neon SIMD Instructions decode */
inline Aarch64Set aarch64::decodeNeon3Same(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg) {
    uint8_t q = bits(machInst, 30);
    uint8_t u = bits(machInst, 29);
    uint8_t size = bits(machInst, 23, 22);
    uint8_t opcode = bits(machInst, 15, 11);

    IntRegIndex vd = (IntRegIndex) (uint8_t) bits(machInst, 4, 0);
    IntRegIndex vn = (IntRegIndex) (uint8_t) bits(machInst, 9, 5);
    IntRegIndex vm = (IntRegIndex) (uint8_t) bits(machInst, 20, 16);
    instReg.DReg[2] = vd;
    instReg.SReg[3] = vn;
    instReg.SReg[4] = vm;

    uint8_t size_q = (size << 1) | q;
    uint8_t sz_q = size_q & 0x3;

    switch (opcode) {
        case 0x00:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UHADD];
                return UHADD;

            }
            else {
                ++instSetCounters[SHADD];
                return SHADD;

            }
        case 0x01:
            if (size_q == 0x6) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UQADD];
                return UQADD;

            }
            else {
                ++instSetCounters[SQADD];
                return SQADD;

            }
        case 0x02:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[URHADD];
                return URHADD;

            }
            else {
                ++instSetCounters[SRHADD];
                return SRHADD;

            }
        case 0x03:
            switch (size) {
                case 0x0:
                    if (u) {
                        if (q) {
                            ++instSetCounters[EorQX];
                            return EorQX;

                        }
                        else {
                            ++instSetCounters[EorDX];
                            return EorDX;

                        }
                    }
                    else {
                        if (q) {
                            ++instSetCounters[AndQX];
                            return AndQX;

                        }
                        else {
                            ++instSetCounters[AndDX];
                            return AndDX;

                        }
                    }
                case 0x1:
                    if (u) {
                        if (q) {
                            ++instSetCounters[BslQX];
                            return BslQX;

                        }
                        else {
                            ++instSetCounters[BslDX];
                            return BslDX;

                        }
                    }
                    else {
                        if (q) {
                            ++instSetCounters[BicQX];
                            return BicQX;

                        }
                        else {
                            ++instSetCounters[BicDX];
                            return BicDX;

                        }
                    }
                case 0x2:
                    if (u) {
                        if (q) {
                            ++instSetCounters[BitQX];
                            return BitQX;

                        }
                        else {
                            ++instSetCounters[BitDX];
                            return BitDX;

                        }
                    }
                    else {
                        if (q) {
                            ++instSetCounters[OrrQX];
                            return OrrQX;

                        }
                        else {
                            ++instSetCounters[OrrDX];
                            return OrrDX;

                        }
                    }
                case 0x3:
                    if (u) {
                        if (q) {
                            ++instSetCounters[BifQX];
                            return BifQX;

                        }
                        else {
                            ++instSetCounters[BifDX];
                            return BifDX;

                        }
                    }
                    else {
                        if (q) {
                            ++instSetCounters[OrnQX];
                            return OrnQX;

                        }
                        else {
                            ++instSetCounters[OrnDX];
                            return OrnDX;

                        }
                    }
            }
        case 0x04:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UHSUB];
                return UHSUB;

            }
            else {
                ++instSetCounters[SHSUB];
                return SHSUB;

            }
        case 0x05:
            if (size_q == 0x6) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UQSUB];
                return UQSUB;

            }
            else {
                ++instSetCounters[SQSUB];
                return SQSUB;

            }
        case 0x06:
            if (size_q == 0x6) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[CMHI];
                return CMHI;

            }
            else {
                ++instSetCounters[CMGT];
                return CMGT;

            }
        case 0x07:
            if (size_q == 0x6) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[CMHS];
                return CMHS;

            }
            else {
                ++instSetCounters[CMGE];
                return CMGE;

            }
        case 0x08:
            if (size_q == 0x6) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[USHL];
                return USHL;

            }
            else {
                ++instSetCounters[SSHL];
                return SSHL;

            }
        case 0x09:
            if (size_q == 0x6) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UQSHL];
                return UQSHL;

            }
            else {
                ++instSetCounters[SQSHL];
                return SQSHL;

            }
        case 0x0a:
            if (size_q == 0x6) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[URSHL];
                return URSHL;

            }
            else {
                ++instSetCounters[SRSHL];
                return SRSHL;

            }
        case 0x0b:
            if (size_q == 0x6) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UQRSHL];
                return UQRSHL;

            }
            else {
                ++instSetCounters[SQRSHL];
                return SQRSHL;

            }
        case 0x0c:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UMAX];
                return UMAX;

            }
            else {
                ++instSetCounters[SMAX];
                return SMAX;

            }
        case 0x0d:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UMIN];
                return UMIN;

            }
            else {
                ++instSetCounters[SMIN];
                return SMIN;

            }
        case 0x0e:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UABD];
                return UABD;

            }
            else {
                ++instSetCounters[SABD];
                return SABD;

            }
        case 0x0f:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UABA];
                return UABA;

            }
            else {
                ++instSetCounters[SABA];
                return SABA;

            }
        case 0x10:
            if (size_q == 0x6) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[SUB];
                return SUB;

            }
            else {
                ++instSetCounters[ADD];
                return ADD;

            }
        case 0x11:
            if (size_q == 0x6) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[CMEQ];
                return CMEQ;

            }
            else {
                ++instSetCounters[CMTST];
                return CMTST;

            }
        case 0x12:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[MLS];
                return MLS;

            }
            else {
                ++instSetCounters[MLA];
                return MLA;

            }
        case 0x13:
            if (size == 0x3 || (size != 0x0 && bits(machInst, 29))) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                if (q) {
                    ++instSetCounters[PmulQX];
                    return PmulQX;

                }
                else {
                    ++instSetCounters[PmulDX];
                    return PmulDX;

                }
            }
            else {
                {
                    ++instSetCounters[MUL];
                    return MUL;

                }
            }
        case 0x14:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UMAXP];
                return UMAXP;

            }
            else {
                ++instSetCounters[SMAXP];
                return SMAXP;

            }
        case 0x15:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UMINP];
                return UMINP;

            }
            else {
                ++instSetCounters[SMINP];
                return SMINP;

            }
        case 0x16:
            if (size == 0x3 || size == 0x0) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                if (q) {
                    ++instSetCounters[SQRDMULH];
                    return SQRDMULH;

                }
                else {
                    ++instSetCounters[SQDMULH];
                    return SQDMULH;

                }
            }
            else {
                if (q) {
                    ++instSetCounters[SQRDMULH];
                    return SQRDMULH;

                }
                else {
                    ++instSetCounters[SQDMULH];
                    return SQDMULH;

                }
            }
        case 0x17:
            if (u || size_q == 0x6) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[ADDP];
                return ADDP;

            }
        case 0x18:
            if (sz_q == 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (size < 0x2) {
                if (u) {
                    ++instSetCounters[FMAXNMP];
                    return FMAXNMP;

                }
                else {
                    ++instSetCounters[FMAXNM];
                    return FMAXNM;

                }
            }
            else {
                if (u) {
                    ++instSetCounters[FMINNMP];
                    return FMINNMP;

                }
                else {
                    ++instSetCounters[FMINNM];
                    return FMINNM;

                }
            }
        case 0x19:
            if (size < 0x2) {
                if (u || sz_q == 0x2) {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
                else {
                    ++instSetCounters[FMLA];
                    return FMLA;

                }
            }
            else {
                if (u || sz_q == 0x2) {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
                else {
                    ++instSetCounters[FMLS];
                    return FMLS;

                }
            }
        case 0x1a:
            if (sz_q == 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (size < 0x2) {
                if (u) {
                    ++instSetCounters[FADDP];
                    return FADDP;

                }
                else {
                    ++instSetCounters[FADD];
                    return FADD;

                }
            }
            else {
                if (u) {
                    ++instSetCounters[FABD];
                    return FABD;

                }
                else {
                    ++instSetCounters[FSUB];
                    return FSUB;

                }
            }
        case 0x1b:
            if (size < 0x2 && sz_q != 0x2) {
                if (u) {
                    ++instSetCounters[FMUL];
                    return FMUL;

                }
                else {
                    ++instSetCounters[FMULX];
                    return FMULX;

                }
            }
            else {
                {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
        case 0x1c:
            if (size < 0x2) {
                if (u) {
                    ++instSetCounters[FCMGE];
                    return FCMGE;

                }
                else {
                    ++instSetCounters[FCMEQ];
                    return FCMEQ;

                }
            }
            else {
                if (u) {
                    ++instSetCounters[FCMGT];
                    return FCMGT;

                }
                else {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
        case 0x1d:
            if (size < 0x2) {
                if (u) {
                    ++instSetCounters[FACGE];
                    return FACGE;

                }
                else {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
            else {
                if (u) {
                    ++instSetCounters[FACGT];
                    return FACGT;

                }
                else {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
        case 0x1e:
            if (sz_q == 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (size < 0x2) {
                if (u) {
                    ++instSetCounters[FMAXP];
                    return FMAXP;

                }
                else {
                    ++instSetCounters[FMAX];
                    return FMAX;

                }
            }
            else {
                if (u) {
                    ++instSetCounters[FMINP];
                    return FMINP;

                }
                else {
                    ++instSetCounters[FMIN];
                    return FMIN;

                }
            }
        case 0x1f:
            if (sz_q == 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (size < 0x2) {
                if (u) {
                    ++instSetCounters[FDIV];
                    return FDIV;

                }
                else {
                    ++instSetCounters[FRECPS];
                    return FRECPS;

                }
            }
            else {
                if (u) {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
                else {
                    ++instSetCounters[FRSQRTS];
                    return FRSQRTS;

                }
            }
        default: {
            ++instSetCounters[Unknown64];
            return Unknown64;

        }
    }
}

inline Aarch64Set aarch64::decodeNeon3Diff(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg) {
    uint8_t q = bits(machInst, 30);
    uint8_t u = bits(machInst, 29);
    uint8_t size = bits(machInst, 23, 22);
    uint8_t opcode = bits(machInst, 15, 12);

    IntRegIndex vd = (IntRegIndex) (uint8_t) bits(machInst, 4, 0);
    IntRegIndex vn = (IntRegIndex) (uint8_t) bits(machInst, 9, 5);
    IntRegIndex vm = (IntRegIndex) (uint8_t) bits(machInst, 20, 16);
    instReg.DReg[2] = vd;
    instReg.SReg[3] = vn;
    instReg.SReg[4] = vm;

    switch (opcode) {
        case 0x0:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UADDL];
                return UADDL;

            }
            else {
                ++instSetCounters[SADDL];
                return SADDL;

            }
        case 0x1:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UADDW];
                return UADDW;

            }
            else {
                ++instSetCounters[SADDW];
                return SADDW;

            }
        case 0x2:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[USUBL];
                return USUBL;

            }
            else {
                ++instSetCounters[SSUBL];
                return SSUBL;

            }
        case 0x3:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[USUBW];
                return USUBW;

            }
            else {
                ++instSetCounters[SSUBW];
                return SSUBW;

            }
        case 0x4:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[RADDHN];
                return RADDHN;

            }
            else {
                ++instSetCounters[ADDHN];
                return ADDHN;

            }
        case 0x5:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UABAL];
                return UABAL;

            }
            else {
                ++instSetCounters[SABAL];
                return SABAL;

            }
        case 0x6:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[RSUBHN];
                return RSUBHN;

            }
            else {
                ++instSetCounters[SUBHN];
                return SUBHN;

            }
        case 0x7:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UABDL];
                return UABDL;

            }
            else {
                ++instSetCounters[SABDL];
                return SABDL;

            }
        case 0x8:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UMLAL];
                return UMLAL;

            }
            else {
                ++instSetCounters[SMLAL];
                return SMLAL;

            }
        case 0x9:
            if (u || (size == 0x0 || size == 0x3)) {
                {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
            else {
                if (q) {
                    {
                        ++instSetCounters[SQDMLAL];
                        return SQDMLAL;

                    }
                }
                else {
                    {
                        ++instSetCounters[SQDMLAL];
                        return SQDMLAL;

                    }
                }
            }
        case 0xa:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UMLSL];
                return UMLSL;

            }
            else {
                ++instSetCounters[SMLSL];
                return SMLSL;

            }
        case 0xb:
            if (u || (size == 0x0 || size == 0x3)) {
                {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
            else {
                if (q) {
                    {
                        ++instSetCounters[SQDMLSL];
                        return SQDMLSL;

                    }
                }
                else {
                    {
                        ++instSetCounters[SQDMLSL];
                        return SQDMLSL;

                    }
                }
            }
        case 0xc:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UMULL];
                return UMULL;

            }
            else {
                ++instSetCounters[SMULL];
                return SMULL;

            }
        case 0xd:
            if (u || (size == 0x0 || size == 0x3)) {
                {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
            else {
                if (q) {
                    {
                        ++instSetCounters[SQDMULL];
                        return SQDMULL;

                    }
                }
                else {
                    {
                        ++instSetCounters[SQDMULL];
                        return SQDMULL;

                    }
                }
            }
        case 0xe:
            if (u || size != 0) {
                {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
            else {
                if (q) {
                    ++instSetCounters[Pmull2X];
                    return Pmull2X;

                }
                else {
                    ++instSetCounters[PmullX];
                    return PmullX;

                }
            }
        default: {
            ++instSetCounters[Unknown64];
            return Unknown64;

        }
    }
}

inline Aarch64Set aarch64::decodeNeon2RegMisc(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg) {
    uint8_t q = bits(machInst, 30);
    uint8_t u = bits(machInst, 29);
    uint8_t size = bits(machInst, 23, 22);
    uint8_t opcode = bits(machInst, 16, 12);

    IntRegIndex vd = (IntRegIndex) (uint8_t) bits(machInst, 4, 0);
    IntRegIndex vn = (IntRegIndex) (uint8_t) bits(machInst, 9, 5);
    instReg.DReg[2] = vd;
    instReg.SReg[3] = vn;

    uint8_t size_q = (size << 1) | q;
    uint8_t sz_q = size_q & 0x3;
    uint8_t op = (uint8_t) ((bits(machInst, 12) << 1) |
                            bits(machInst, 29));
    uint8_t switchVal = opcode | ((u ? 1 : 0) << 5);

    switch (switchVal) {
        case 0x00:
            if (op + size >= 3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[REV64];
                return REV64;

            }
        case 0x01:
            if (op + size >= 3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (q) {
                ++instSetCounters[Rev16QX];
                return Rev16QX;

            }
            else {
                ++instSetCounters[Rev16DX];
                return Rev16DX;

            }
        case 0x02:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[SADDLP];
                return SADDLP;

            }
        case 0x03:
            if (size_q == 0x6) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[SUQADD];
                return SUQADD;

            }
        case 0x04:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[CLS];
                return CLS;

            }
        case 0x05:
            if (size != 0x0) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (q) {
                ++instSetCounters[CntQX];
                return CntQX;

            }
            else {
                ++instSetCounters[CntDX];
                return CntDX;

            }
        case 0x06:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[SADALP];
                return SADALP;

            }
        case 0x07:
            if (size_q == 0x6) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[SQABS];
                return SQABS;

            }
        case 0x08:
            if (size_q == 0x6) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[CMGT];
                return CMGT;

            }
        case 0x09:
            if (size_q == 0x6) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[CMEQ];
                return CMEQ;

            }
        case 0x0a:
            if (size_q == 0x6) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[CMLT];
                return CMLT;

            }
        case 0x0b:
            if (size_q == 0x6) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[ABS];
                return ABS;

            }
        case 0x0c:
            if (size < 0x2 || sz_q == 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[FCMGT];
                return FCMGT;

            }
        case 0x0d:
            if (size < 0x2 || sz_q == 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[FCMEQ];
                return FCMEQ;

            }
        case 0x0e:
            if (size < 0x2 || sz_q == 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[FCMLT];
                return FCMLT;

            }
        case 0x0f:
            if (size < 0x2 || sz_q == 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[FABS];
                return FABS;

            }
        case 0x12:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[XTN];
                return XTN;

            }
        case 0x14:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[SQXTN];
                return SQXTN;

            }
        case 0x16:
            if (size > 0x1) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (q) {
                if (size) {
                    ++instSetCounters[Fcvtn2X];
                    return Fcvtn2X;

                }
                else {
                    ++instSetCounters[Fcvtn2X];
                    return Fcvtn2X;

                }
            }
            else {
                if (size) {
                    ++instSetCounters[FcvtnX];
                    return FcvtnX;

                }
                else {
                    ++instSetCounters[FcvtnX];
                    return FcvtnX;

                }
            }
        case 0x17:
            if (size > 0x1) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (q) {
                if (size) {
                    ++instSetCounters[Fcvtl2X];
                    return Fcvtl2X;

                }
                else {
                    ++instSetCounters[Fcvtl2X];
                    return Fcvtl2X;

                }
            }
            else {
                if (size) {
                    ++instSetCounters[FcvtlX];
                    return FcvtlX;

                }
                else {
                    ++instSetCounters[FcvtlX];
                    return FcvtlX;

                }
            }
        case 0x18:
            if (sz_q == 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (size < 0x2) {
                ++instSetCounters[FRINTN];
                return FRINTN;

            }
            else {
                ++instSetCounters[FRINTP];
                return FRINTP;

            }
        case 0x19:
            if (sz_q == 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (size < 0x2) {
                ++instSetCounters[FRINTM];
                return FRINTM;

            }
            else {
                ++instSetCounters[FRINTZ];
                return FRINTZ;

            }
        case 0x1a:
            if (sz_q == 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (size < 0x2) {
                ++instSetCounters[FCVTNS];
                return FCVTNS;

            }
            else {
                ++instSetCounters[FCVTPS];
                return FCVTPS;

            }
        case 0x1b:
            if (sz_q == 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (size < 0x2) {
                ++instSetCounters[FCVTMS];
                return FCVTMS;

            }
            else {
                ++instSetCounters[FCVTZS];
                return FCVTZS;

            }
        case 0x1c:
            if (size < 0x2) {
                if (sz_q == 0x2) {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
                {
                    ++instSetCounters[FCVTAS];
                    return FCVTAS;

                }
            }
            else {
                if (size & 0x1) {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
                if (q) {
                    ++instSetCounters[UrecpeQX];
                    return UrecpeQX;

                }
                else {
                    ++instSetCounters[UrecpeDX];
                    return UrecpeDX;

                }
            }
        case 0x1d:
            if (sz_q == 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (size < 0x2) {
                if (q) {
                    if (size & 0x1) {
                        ++instSetCounters[ScvtfIntDQX];
                        return ScvtfIntDQX;

                    }
                    else {
                        ++instSetCounters[ScvtfIntSQX];
                        return ScvtfIntSQX;

                    }
                }
                else {
                    if (size & 0x1) {
                        ++instSetCounters[Unknown64];
                        return Unknown64;

                    }
                    else {
                        ++instSetCounters[ScvtfIntDX];
                        return ScvtfIntDX;

                    }
                }
            }
            else {
                {
                    ++instSetCounters[FRECPE];
                    return FRECPE;

                }
            }
        case 0x20:
            if (op + size >= 3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (q) {
                if (size & 0x1) {
                    ++instSetCounters[Rev32QX];
                    return Rev32QX;

                }
                else {
                    ++instSetCounters[Rev32QX];
                    return Rev32QX;

                }
            }
            else {
                if (size & 0x1) {
                    ++instSetCounters[Rev32DX];
                    return Rev32DX;

                }
                else {
                    ++instSetCounters[Rev32DX];
                    return Rev32DX;

                }
            }
        case 0x22:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[UADDLP];
                return UADDLP;

            }
        case 0x23:
            if (size_q == 0x6) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[USQADD];
                return USQADD;

            }
            {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
        case 0x24:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[CLZ];
                return CLZ;

            }
        case 0x25:
            if (size == 0x0) {
                if (q) {
                    ++instSetCounters[MvnQX];
                    return MvnQX;

                }
                else {
                    ++instSetCounters[MvnDX];
                    return MvnDX;

                }
            }
            else if (size == 0x1) {
                if (q) {
                    ++instSetCounters[RbitQX];
                    return RbitQX;

                }
                else {
                    ++instSetCounters[RbitDX];
                    return RbitDX;

                }
            }
            else {
                {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
        case 0x26:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[UADALP];
                return UADALP;

            }
        case 0x27:
            if (size_q == 0x6) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[SQNEG];
                return SQNEG;

            }
        case 0x28:
            if (size_q == 0x6) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[CMGE];
                return CMGE;

            }
        case 0x29:
            if (size_q == 0x6) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[CMLE];
                return CMLE;

            }
        case 0x2b:
            if (size_q == 0x6) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[NEG];
                return NEG;

            }
        case 0x2c:
            if (size < 0x2 || sz_q == 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[FCMGE];
                return FCMGE;

            }
        case 0x2d:
            if (size < 0x2 || sz_q == 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[FCMLE];
                return FCMLE;

            }
        case 0x2f:
            if (size < 0x2 || size_q == 0x6) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[FNEG];
                return FNEG;

            }
        case 0x32:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[SQXTUN];
                return SQXTUN;

            }
        case 0x33:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[SHLL];
                return SHLL;

            }
        case 0x34:
            if (size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[UQXTN];
                return UQXTN;

            }
        case 0x36:
            if (size != 0x1) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (q) {
                ++instSetCounters[Fcvtxn2X];
                return Fcvtxn2X;

            }
            else {
                ++instSetCounters[FcvtxnX];
                return FcvtxnX;

            }
        case 0x38:
            if (size > 0x1 || sz_q == 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[FRINTA];
                return FRINTA;

            }
        case 0x39:
            if (sz_q == 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (size < 0x2) {
                ++instSetCounters[FRINTX];
                return FRINTX;

            }
            else {
                ++instSetCounters[FRINTI];
                return FRINTI;

            }
        case 0x3a:
            if (sz_q == 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (size < 0x2) {
                ++instSetCounters[FCVTNU];
                return FCVTNU;

            }
            else {
                ++instSetCounters[FCVTPU];
                return FCVTPU;

            }
        case 0x3b:
            if (sz_q == 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (size < 0x2) {
                ++instSetCounters[FCVTMU];
                return FCVTMU;

            }
            else {
                ++instSetCounters[FCVTZU];
                return FCVTZU;

            }
        case 0x3c:
            if (size < 0x2) {
                {
                    ++instSetCounters[FCVTAU];
                    return FCVTAU;

                }
            }
            else if (size == 0x2) {
                if (q) {
                    ++instSetCounters[UrsqrteQX];
                    return UrsqrteQX;

                }
                else {
                    ++instSetCounters[UrsqrteDX];
                    return UrsqrteDX;

                }
            }
            else {
                {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
        case 0x3d:
            if (sz_q == 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (size < 0x2) {
                ++instSetCounters[UCVTF];
                return UCVTF;

            }
            else {
                ++instSetCounters[FRSQRTE];
                return FRSQRTE;

            }
        case 0x3f:
            if (size < 0x2 || sz_q == 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[FSQRT];
                return FSQRT;

            }
        default: {
            ++instSetCounters[Unknown64];
            return Unknown64;

        }
    }
}

inline Aarch64Set aarch64::decodeNeonAcrossLanes(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg) {
    uint8_t q = bits(machInst, 30);
    uint8_t u = bits(machInst, 29);
    uint8_t size = bits(machInst, 23, 22);
    uint8_t opcode = bits(machInst, 16, 12);

    IntRegIndex vd = (IntRegIndex) (uint8_t) bits(machInst, 4, 0);
    IntRegIndex vn = (IntRegIndex) (uint8_t) bits(machInst, 9, 5);
    instReg.DReg[2] = vd;
    instReg.SReg[3] = vn;

    uint8_t size_q = (size << 1) | q;
    uint8_t sz_q = size_q & 0x3;
    uint8_t switchVal = opcode | ((u ? 1 : 0) << 5);

    switch (switchVal) {
        case 0x03:
            if (size_q == 0x4 || size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[SADDLV];
                return SADDLV;

            }
        case 0x0a:
            if (size_q == 0x4 || size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[SMAXV];
                return SMAXV;

            }
        case 0x1a:
            if (size_q == 0x4 || size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[SMINV];
                return SMINV;

            }
        case 0x1b:
            if (size_q == 0x4 || size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[ADDV];
                return ADDV;

            }
        case 0x23:
            if (size_q == 0x4 || size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[UADDLV];
                return UADDLV;

            }
        case 0x2a:
            if (size_q == 0x4 || size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[UMAXV];
                return UMAXV;

            }
        case 0x2c:
            if (sz_q != 0x1) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (size < 0x2) {
                if (q) {
                    ++instSetCounters[FmaxnmvQX];
                    return FmaxnmvQX;

                }
                else {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
            else {
                if (q) {
                    ++instSetCounters[FminnmvQX];
                    return FminnmvQX;

                }
                else {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
        case 0x2f:
            if (sz_q != 0x1) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (size < 0x2) {
                if (q) {
                    ++instSetCounters[FmaxvQX];
                    return FmaxvQX;

                }
                else {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
            else {
                if (q) {
                    ++instSetCounters[FminvQX];
                    return FminvQX;

                }
                else {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
        case 0x3a:
            if (size_q == 0x4 || size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            {
                ++instSetCounters[UMINV];
                return UMINV;

            }
        default: {
            ++instSetCounters[Unknown64];
            return Unknown64;

        }
    }
}

inline Aarch64Set aarch64::decodeNeonCopy(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg) {
    uint8_t q = bits(machInst, 30);
    uint8_t op = bits(machInst, 29);
    uint8_t imm5 = bits(machInst, 20, 16);
    uint8_t imm4 = bits(machInst, 14, 11);

    IntRegIndex vd = (IntRegIndex) (uint8_t) bits(machInst, 4, 0);
    IntRegIndex vn = (IntRegIndex) (uint8_t) bits(machInst, 9, 5);
    instReg.DReg[2] = vd;
    instReg.SReg[3] = vn;

    uint8_t imm5_pos = findLsbSet(imm5);
    uint8_t index1 = 0, index2 = 0;

    if (op) {
        if (!q || (imm4 & mask(imm5_pos))) {
            ++instSetCounters[Unknown64];
            return Unknown64;

        }

        index1 = bits(imm5, 4, imm5_pos + 1);  // dst
        index2 = bits(imm4, 3, imm5_pos);  // src

        switch (imm5_pos) {
            case 0: {
                ++instSetCounters[InsElemX];
                return InsElemX;

            }
            case 1: {
                ++instSetCounters[InsElemX];
                return InsElemX;

            }
            case 2: {
                ++instSetCounters[InsElemX];
                return InsElemX;

            }
            case 3: {
                ++instSetCounters[InsElemX];
                return InsElemX;

            }
            default: {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
        }
    }

    switch (imm4) {
        case 0x0:
            index1 = bits(imm5, 4, imm5_pos + 1);
            switch (imm5_pos) {
                case 0:
                    if (q) {
                        ++instSetCounters[DupElemQX];
                        return DupElemQX;

                    }
                    else {
                        ++instSetCounters[DupElemDX];
                        return DupElemDX;

                    }
                case 1:
                    if (q) {
                        ++instSetCounters[DupElemQX];
                        return DupElemQX;

                    }
                    else {
                        ++instSetCounters[DupElemDX];
                        return DupElemDX;

                    }
                case 2:
                    if (q) {
                        ++instSetCounters[DupElemQX];
                        return DupElemQX;

                    }
                    else {
                        ++instSetCounters[DupElemDX];
                        return DupElemDX;

                    }
                case 3:
                    if (q) {
                        ++instSetCounters[DupElemQX];
                        return DupElemQX;

                    }
                    else {
                        ++instSetCounters[Unknown64];
                        return Unknown64;

                    }
                default: {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
        case 0x1:
            switch (imm5) {
                case 0x1:
                    if (q) {
                        ++instSetCounters[DupGprWQX];
                        return DupGprWQX;

                    }
                    else {
                        ++instSetCounters[DupGprWDX];
                        return DupGprWDX;

                    }
                case 0x2:
                    if (q) {
                        ++instSetCounters[DupGprWQX];
                        return DupGprWQX;

                    }
                    else {
                        ++instSetCounters[DupGprWDX];
                        return DupGprWDX;

                    }
                case 0x4:
                    if (q) {
                        ++instSetCounters[DupGprWQX];
                        return DupGprWQX;

                    }
                    else {
                        ++instSetCounters[DupGprWDX];
                        return DupGprWDX;

                    }
                case 0x8:
                    if (q) {
                        ++instSetCounters[DupGprXQX];
                        return DupGprXQX;

                    }
                    else {
                        ++instSetCounters[Unknown64];
                        return Unknown64;

                    }
            }
        case 0x3:
            index1 = imm5 >> (imm5_pos + 1);
            switch (imm5_pos) {
                case 0: {
                    ++instSetCounters[InsGprWX];
                    return InsGprWX;

                }
                case 1: {
                    ++instSetCounters[InsGprWX];
                    return InsGprWX;

                }
                case 2: {
                    ++instSetCounters[InsGprWX];
                    return InsGprWX;

                }
                case 3: {
                    ++instSetCounters[InsGprXX];
                    return InsGprXX;

                }
                default: {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
        case 0x5:
            index1 = bits(imm5, 4, imm5_pos + 1);
            switch (imm5_pos) {
                case 0:
                    if (q) {
                        ++instSetCounters[SmovXX];
                        return SmovXX;

                    }
                    else {
                        ++instSetCounters[SmovWX];
                        return SmovWX;

                    }
                case 1:
                    if (q) {
                        ++instSetCounters[SmovXX];
                        return SmovXX;

                    }
                    else {
                        ++instSetCounters[SmovWX];
                        return SmovWX;

                    }
                case 2:
                    if (q) {
                        ++instSetCounters[SmovXX];
                        return SmovXX;

                    }
                    else {
                        ++instSetCounters[Unknown64];
                        return Unknown64;

                    }
                default: {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
        case 0x7:
            index1 = imm5 >> (imm5_pos + 1);

            if ((q && imm5_pos != 3) || (!q && imm5_pos >= 3)) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }

            switch (imm5_pos) {
                case 0: {
                    ++instSetCounters[UmovWX];
                    return UmovWX;

                }
                case 1: {
                    ++instSetCounters[UmovWX];
                    return UmovWX;

                }
                case 2: {
                    ++instSetCounters[UmovWX];
                    return UmovWX;

                }
                case 3: {
                    ++instSetCounters[UmovXX];
                    return UmovXX;

                }
                default: {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
        default: {
            ++instSetCounters[Unknown64];
            return Unknown64;

        }
    }
}

inline Aarch64Set aarch64::decodeNeonIndexedElem(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg) {
    uint8_t q = bits(machInst, 30);
    uint8_t u = bits(machInst, 29);
    uint8_t size = bits(machInst, 23, 22);
    uint8_t L = bits(machInst, 21);
    uint8_t M = bits(machInst, 20);
    uint8_t opcode = bits(machInst, 15, 12);
    uint8_t H = bits(machInst, 11);

    IntRegIndex vd = (IntRegIndex) (uint8_t) bits(machInst, 4, 0);
    IntRegIndex vn = (IntRegIndex) (uint8_t) bits(machInst, 9, 5);
    IntRegIndex vm_bf = (IntRegIndex) (uint8_t) bits(machInst, 19, 16);

    uint8_t index = 0;
    uint8_t index_fp = 0;
    uint8_t vmh = 0;
    uint8_t sz = size & 0x1;
    uint8_t sz_q = (sz << 1) | bits(machInst, 30);
    uint8_t sz_L = (sz << 1) | L;

    // Index and 2nd register operand for integer instructions
    if (size == 0x1) {
        index = (H << 2) | (L << 1) | M;
        // vmh = 0;
    }
    else if (size == 0x2) {
        index = (H << 1) | L;
        vmh = M;
    }
    IntRegIndex vm = (IntRegIndex) (uint8_t) (vmh << 4 | vm_bf);

    // Index and 2nd register operand for FP instructions
    vmh = M;
    if ((size & 0x1) == 0) {
        index_fp = (H << 1) | L;
    }
    else if (L == 0) {
        index_fp = H;
    }
    IntRegIndex vm_fp = (IntRegIndex) (uint8_t) (vmh << 4 | vm_bf);

    switch (opcode) {
        case 0x0:
            if (!u || (size == 0x0 || size == 0x3)) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[MLA];
                return MLA;

            }
        case 0x1:
            if (!u && size >= 2 && sz_q != 0x2 && sz_L != 0x3) {
                ++instSetCounters[FMLA];
                return FMLA;

            }
            else {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
        case 0x2:
            if (size == 0x0 || size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UMLAL];
                return UMLAL;

            }
            else {
                ++instSetCounters[SMLAL];
                return SMLAL;

            }
        case 0x3:
            if (u || (size == 0x0 || size == 0x3)) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[SQDMLAL];
                return SQDMLAL;

            }
        case 0x4:
            if (u && !(size == 0x0 || size == 0x3)) {
                ++instSetCounters[MLS];
                return MLS;

            }
            else {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
        case 0x5:
            if (!u && size >= 0x2 && sz_L != 0x3 && sz_q != 0x2) {
                ++instSetCounters[FMLS];
                return FMLS;

            }
            else {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
        case 0x6:
            if (size == 0x0 || size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UMLSL];
                return UMLSL;

            }
            else {
                ++instSetCounters[SMLSL];
                return SMLSL;

            }
        case 0x7:
            if (u || (size == 0x0 || size == 0x3)) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[SQDMLSL];
                return SQDMLSL;

            }
        case 0x8:
            if (u || (size == 0x0 || size == 0x3)) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[MUL];
                return MUL;

            }
        case 0x9:
            if (size >= 2 && sz_q != 0x2 && sz_L != 0x3) {
                if (u) {
                    ++instSetCounters[FMULX];
                    return FMULX;

                }
                else {
                    ++instSetCounters[FMUL];
                    return FMUL;

                }
            }
            else {
                {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
        case 0xa:
            if (size == 0x0 || size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UMULL];
                return UMULL;

            }
            else {
                ++instSetCounters[SMULL];
                return SMULL;

            }
        case 0xb:
            if (u || (size == 0x0 || size == 0x3)) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[SQDMULL];
                return SQDMULL;

            }
        case 0xc:
            if (u || (size == 0x0 || size == 0x3)) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[SQDMULH];
                return SQDMULH;

            }
        case 0xd:
            if (u || (size == 0x0 || size == 0x3)) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[SQRDMULH];
                return SQRDMULH;

            }
        default: {
            ++instSetCounters[Unknown64];
            return Unknown64;

        }
    }
}

inline Aarch64Set aarch64::decodeNeonModImm(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg) {
    uint8_t q = bits(machInst, 30);
    uint8_t op = bits(machInst, 29);
    uint8_t abcdefgh = (bits(machInst, 18, 16) << 5) |
                       bits(machInst, 9, 5);
    uint8_t cmode = bits(machInst, 15, 12);
    uint8_t o2 = bits(machInst, 11);

    IntRegIndex vd = (IntRegIndex) (uint8_t) bits(machInst, 4, 0);
    instReg.DReg[2] = vd;

    if (o2 == 0x1 || (op == 0x1 && cmode == 0xf && !q)) {
        ++instSetCounters[Unknown64];
        return Unknown64;

    }

    bool immValid = true;
    if (!immValid) {
        {
            ++instSetCounters[Unknown64];
            return Unknown64;

        }
    }

    if (op) {
        if (bits(cmode, 3) == 0) {
            if (bits(cmode, 0) == 0) {
                if (q) {
                    ++instSetCounters[MvniQX];
                    return MvniQX;

                }
                else {
                    ++instSetCounters[MvniDX];
                    return MvniDX;

                }
            }
            else {
                if (q) {
                    ++instSetCounters[BicImmQX];
                    return BicImmQX;

                }
                else {
                    ++instSetCounters[BicImmDX];
                    return BicImmDX;

                }
            }
        }
        else {
            if (bits(cmode, 2) == 1) {
                switch (bits(cmode, 1, 0)) {
                    case 0:
                    case 1:
                        if (q) {
                            ++instSetCounters[MvniQX];
                            return MvniQX;

                        }
                        else {
                            ++instSetCounters[MvniDX];
                            return MvniDX;

                        }
                    case 2:
                        if (q) {
                            ++instSetCounters[MoviQX];
                            return MoviQX;

                        }
                        else {
                            ++instSetCounters[MoviDX];
                            return MoviDX;

                        }
                    case 3:
                        if (q) {
                            ++instSetCounters[FmovQX];
                            return FmovQX;

                        }
                        else {
                            ++instSetCounters[MoviDX];
                            return MoviDX;

                        }
                }
            }
            else {
                if (bits(cmode, 0) == 0) {
                    if (q) {
                        ++instSetCounters[MvniQX];
                        return MvniQX;

                    }
                    else {
                        ++instSetCounters[MvniDX];
                        return MvniDX;

                    }
                }
                else {
                    if (q) {
                        ++instSetCounters[BicImmQX];
                        return BicImmQX;

                    }
                    else {
                        ++instSetCounters[BicImmDX];
                        return BicImmDX;

                    }
                }
            }
        }
    }
    else {
        if (bits(cmode, 3) == 0) {
            if (bits(cmode, 0) == 0) {
                if (q) {
                    ++instSetCounters[MoviQX];
                    return MoviQX;

                }
                else {
                    ++instSetCounters[MoviDX];
                    return MoviDX;

                }
            }
            else {
                if (q) {
                    ++instSetCounters[OrrImmQX];
                    return OrrImmQX;

                }
                else {
                    ++instSetCounters[OrrImmDX];
                    return OrrImmDX;

                }
            }
        }
        else {
            if (bits(cmode, 2) == 1) {
                if (bits(cmode, 1, 0) == 0x3) {
                    if (q) {
                        ++instSetCounters[FmovQX];
                        return FmovQX;

                    }
                    else {
                        ++instSetCounters[FmovDX];
                        return FmovDX;

                    }
                }
                else {
                    if (q) {
                        ++instSetCounters[MoviQX];
                        return MoviQX;

                    }
                    else {
                        ++instSetCounters[MoviDX];
                        return MoviDX;

                    }
                }
            }
            else {
                if (bits(cmode, 0) == 0) {
                    if (q) {
                        ++instSetCounters[MoviQX];
                        return MoviQX;

                    }
                    else {
                        ++instSetCounters[MoviDX];
                        return MoviDX;

                    }
                }
                else {
                    if (q) {
                        ++instSetCounters[OrrImmQX];
                        return OrrImmQX;

                    }
                    else {
                        ++instSetCounters[OrrImmDX];
                        return OrrImmDX;

                    }
                }
            }
        }
    }
    {
        ++instSetCounters[Unknown64];
        return Unknown64;

    }
}

inline Aarch64Set aarch64::decodeNeonShiftByImm(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg) {
    uint8_t q = bits(machInst, 30);
    uint8_t u = bits(machInst, 29);
    uint8_t immh = bits(machInst, 22, 19);
    uint8_t immb = bits(machInst, 18, 16);
    uint8_t opcode = bits(machInst, 15, 11);

    IntRegIndex vd = (IntRegIndex) (uint8_t) bits(machInst, 4, 0);
    IntRegIndex vn = (IntRegIndex) (uint8_t) bits(machInst, 9, 5);
    instReg.DReg[2] = vd;
    instReg.SReg[3] = vn;

    uint8_t immh3 = bits(machInst, 22);
    uint8_t immh3_q = (immh3 << 1) | q;
    uint8_t op_u = (bits(machInst, 12) << 1) | u;
    uint8_t size = findMsbSet(immh);
    int shiftAmt = 0;

    switch (opcode) {
        case 0x00:
            if (immh3_q == 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[USHR];
                return USHR;

            }
            else {
                ++instSetCounters[SSHR];
                return SSHR;

            }
        case 0x02:
            if (immh3_q == 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[USRA];
                return USRA;

            }
            else {
                ++instSetCounters[SSRA];
                return SSRA;

            }
        case 0x04:
            if (immh3_q == 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[URSHR];
                return URSHR;

            }
            else {
                ++instSetCounters[SRSHR];
                return SRSHR;

            }
        case 0x06:
            if (immh3_q == 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[URSRA];
                return URSRA;

            }
            else {
                ++instSetCounters[SRSRA];
                return SRSRA;

            }
        case 0x08:
            if (u && !(immh3_q == 0x2)) {
                {
                    ++instSetCounters[SRI];
                    return SRI;

                }
            }
            else {
                {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
        case 0x0a:
            if (immh3_q == 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            shiftAmt = ((immh << 3) | immb) - (8 << size);
            if (u) {
                ++instSetCounters[SLI];
                return SLI;

            }
            else {
                ++instSetCounters[SHL];
                return SHL;

            }
        case 0x0c:
            if (u && !(immh3_q == 0x2 || op_u == 0x0)) {
                shiftAmt = ((immh << 3) | immb) - (8 << size);
                {
                    ++instSetCounters[SQSHLU];
                    return SQSHLU;

                }
            }
            else {
                {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
        case 0x0e:
            if (immh3_q == 0x2 || op_u == 0x0) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            shiftAmt = ((immh << 3) | immb) - (8 << size);
            if (u) {
                ++instSetCounters[UQSHL];
                return UQSHL;

            }
            else {
                ++instSetCounters[SQSHL];
                return SQSHL;

            }
        case 0x10:
            if (immh3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[SQSHRUN];
                return SQSHRUN;

            }
            else {
                ++instSetCounters[SHRN];
                return SHRN;

            }
        case 0x11:
            if (immh3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[SQRSHRUN];
                return SQRSHRUN;

            }
            else {
                ++instSetCounters[RSHRN];
                return RSHRN;

            }
        case 0x12:
            if (immh3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UQSHRN];
                return UQSHRN;

            }
            else {
                ++instSetCounters[SQSHRN];
                return SQSHRN;

            }
        case 0x13:
            if (immh3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UQRSHRN];
                return UQRSHRN;

            }
            else {
                ++instSetCounters[SQRSHRN];
                return SQRSHRN;

            }
        case 0x14:
            if (immh3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            shiftAmt = ((immh << 3) | immb) - (8 << size);
            if (u) {
                ++instSetCounters[USHLL];
                return USHLL;

            }
            else {
                ++instSetCounters[SSHLL];
                return SSHLL;

            }
        case 0x1c:
            if (immh < 0x4 || immh3_q == 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                {
                    ++instSetCounters[UCVTF];
                    return UCVTF;

                }
            }
            else {
                if (q) {
                    if (size & 0x1) {
                        ++instSetCounters[ScvtfFixedDQX];
                        return ScvtfFixedDQX;

                    }
                    else {
                        ++instSetCounters[ScvtfFixedSQX];
                        return ScvtfFixedSQX;

                    }
                }
                else {
                    if (size & 0x1) {
                        ++instSetCounters[Unknown64];
                        return Unknown64;

                    }
                    else {
                        ++instSetCounters[ScvtfFixedDX];
                        return ScvtfFixedDX;

                    }
                }
            }
        case 0x1f:
            if (immh < 0x4 || immh3_q == 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[FCVTZU];
                return FCVTZU;

            }
            else {
                ++instSetCounters[FCVTZS];
                return FCVTZS;

            }
        default: {
            ++instSetCounters[Unknown64];
            return Unknown64;

        }
    }
}

inline Aarch64Set aarch64::decodeNeonTblTbx(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg) {
    uint8_t q = bits(machInst, 30);

    IntRegIndex vd = (IntRegIndex) (uint8_t) bits(machInst, 4, 0);
    IntRegIndex vn = (IntRegIndex) (uint8_t) bits(machInst, 9, 5);
    IntRegIndex vm = (IntRegIndex) (uint8_t) bits(machInst, 20, 16);
    instReg.DReg[2] = vd;
    instReg.SReg[3] = vn;
    instReg.SReg[4] = vm;

    uint8_t switchVal = bits(machInst, 14, 12);

    switch (switchVal) {
        case 0x0:
            if (q) {
                ++instSetCounters[Tbl1QX];
                return Tbl1QX;

            }
            else {
                ++instSetCounters[Tbl1DX];
                return Tbl1DX;

            }
        case 0x1:
            if (q) {
                ++instSetCounters[Tbx1QX];
                return Tbx1QX;

            }
            else {
                ++instSetCounters[Tbx1DX];
                return Tbx1DX;

            }
        case 0x2:
            if (q) {
                ++instSetCounters[Tbl2QX];
                return Tbl2QX;

            }
            else {
                ++instSetCounters[Tbl2DX];
                return Tbl2DX;

            }
        case 0x3:
            if (q) {
                ++instSetCounters[Tbx2QX];
                return Tbx2QX;

            }
            else {
                ++instSetCounters[Tbx2DX];
                return Tbx2DX;

            }
        case 0x4:
            if (q) {
                ++instSetCounters[Tbl3QX];
                return Tbl3QX;

            }
            else {
                ++instSetCounters[Tbl3DX];
                return Tbl3DX;

            }
        case 0x5:
            if (q) {
                ++instSetCounters[Tbx3QX];
                return Tbx3QX;

            }
            else {
                ++instSetCounters[Tbx3DX];
                return Tbx3DX;

            }
        case 0x6:
            if (q) {
                ++instSetCounters[Tbl4QX];
                return Tbl4QX;

            }
            else {
                ++instSetCounters[Tbl4DX];
                return Tbl4DX;

            }
        case 0x7:
            if (q) {
                ++instSetCounters[Tbx4QX];
                return Tbx4QX;

            }
            else {
                ++instSetCounters[Tbx4DX];
                return Tbx4DX;

            }
        default: {
            ++instSetCounters[Unknown64];
            return Unknown64;

        }
    }

    {
        ++instSetCounters[Unknown64];
        return Unknown64;

    }
}

inline Aarch64Set aarch64::decodeNeonZipUzpTrn(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg) {
    uint8_t q = bits(machInst, 30);
    uint8_t opcode = bits(machInst, 14, 12);

    IntRegIndex vd = (IntRegIndex) (uint8_t) bits(machInst, 4, 0);
    IntRegIndex vn = (IntRegIndex) (uint8_t) bits(machInst, 9, 5);
    IntRegIndex vm = (IntRegIndex) (uint8_t) bits(machInst, 20, 16);
    instReg.DReg[2] = vd;
    instReg.SReg[3] = vn;
    instReg.SReg[4] = vm;

    switch (opcode) {
        case 0x1: {
            ++instSetCounters[UZP1];
            return UZP1;

        }
        case 0x2: {
            ++instSetCounters[TRN1];
            return TRN1;

        }
        case 0x3: {
            ++instSetCounters[ZIP1];
            return ZIP1;

        }
        case 0x5: {
            ++instSetCounters[UZP2];
            return UZP2;

        }
        case 0x6: {
            ++instSetCounters[TRN2];
            return TRN2;

        }
        case 0x7: {
            ++instSetCounters[ZIP2];
            return ZIP2;

        }
        default: {
            ++instSetCounters[Unknown64];
            return Unknown64;

        }
    }
    {
        ++instSetCounters[Unknown64];
        return Unknown64;

    }
}

inline Aarch64Set aarch64::decodeNeonExt(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg) {
    uint8_t q = bits(machInst, 30);
    uint8_t op2 = bits(machInst, 23, 22);
    uint8_t imm4 = bits(machInst, 14, 11);

    IntRegIndex vd = (IntRegIndex) (uint8_t) bits(machInst, 4, 0);
    IntRegIndex vn = (IntRegIndex) (uint8_t) bits(machInst, 9, 5);
    IntRegIndex vm = (IntRegIndex) (uint8_t) bits(machInst, 20, 16);
    instReg.DReg[2] = vd;
    instReg.SReg[3] = vn;
    instReg.SReg[4] = vm;

    if (op2 != 0 || (q == 0x0 && bits(imm4, 3) == 0x1)) {
        ++instSetCounters[Unknown64];
        return Unknown64;

    }

    uint8_t index = q ? imm4 : imm4 & 0x7;

    if (q) {
        {
            ++instSetCounters[ExtQX];
            return ExtQX;

        }
    }
    else {
        {
            ++instSetCounters[ExtDX];
            return ExtDX;

        }
    }
}

inline Aarch64Set aarch64::decodeNeonSc3Same(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg) {
    uint8_t u = bits(machInst, 29);
    uint8_t size = bits(machInst, 23, 22);
    uint8_t opcode = bits(machInst, 15, 11);
    uint8_t s = bits(machInst, 11);

    IntRegIndex vd = (IntRegIndex) (uint8_t) bits(machInst, 4, 0);
    IntRegIndex vn = (IntRegIndex) (uint8_t) bits(machInst, 9, 5);
    IntRegIndex vm = (IntRegIndex) (uint8_t) bits(machInst, 20, 16);
    instReg.DReg[2] = vd;
    instReg.SReg[3] = vn;
    instReg.SReg[4] = vm;

    switch (opcode) {
        case 0x01:
            if (u) {
                ++instSetCounters[UQADDS];
                return UQADDS;

            }
            else {
                ++instSetCounters[SQADDS];
                return SQADDS;

            }
        case 0x05:
            if (u) {
                ++instSetCounters[UQSUBS];
                return UQSUBS;

            }
            else {
                ++instSetCounters[SQSUBS];
                return SQSUBS;

            }
        case 0x06:
            if (size != 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[CmhiDX];
                return CmhiDX;

            }
            else {
                ++instSetCounters[CmgtDX];
                return CmgtDX;

            }
        case 0x07:
            if (size != 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[CmhsDX];
                return CmhsDX;

            }
            else {
                ++instSetCounters[CmgeDX];
                return CmgeDX;

            }
        case 0x08:
            if (!s && size != 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UshlDX];
                return UshlDX;

            }
            else {
                ++instSetCounters[SshlDX];
                return SshlDX;

            }
        case 0x09:
            if (!s && size != 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UQSHLS];
                return UQSHLS;

            }
            else {
                ++instSetCounters[SQSHLS];
                return SQSHLS;

            }
        case 0x0a:
            if (!s && size != 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UrshlDX];
                return UrshlDX;

            }
            else {
                ++instSetCounters[SrshlDX];
                return SrshlDX;

            }
        case 0x0b:
            if (!s && size != 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UQRSHLS];
                return UQRSHLS;

            }
            else {
                ++instSetCounters[SQRSHLS];
                return SQRSHLS;

            }
        case 0x10:
            if (size != 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[SubDX];
                return SubDX;

            }
            else {
                ++instSetCounters[AddDX];
                return AddDX;

            }
        case 0x11:
            if (size != 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[CmeqDX];
                return CmeqDX;

            }
            else {
                ++instSetCounters[CmtstDX];
                return CmtstDX;

            }
        case 0x16:
            if (size == 0x3 || size == 0x0) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[SQRDMULHS];
                return SQRDMULHS;

            }
            else {
                ++instSetCounters[SQDMULHS];
                return SQDMULHS;

            }
        case 0x1a:
            if (!u || size < 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[FABDS];
                return FABDS;

            }
        case 0x1b:
            if (u || size > 0x1) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[FCMEQS];
                return FCMEQS;

            }
        case 0x1c:
            if (size < 0x2) {
                if (u) {
                    ++instSetCounters[FCMGES];
                    return FCMGES;

                }
                else {
                    ++instSetCounters[FCMEQS];
                    return FCMEQS;

                }
            }
            else {
                if (u) {
                    ++instSetCounters[FCMGTS];
                    return FCMGTS;

                }
                else {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
        case 0x1d:
            if (!u) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (size < 0x2) {
                ++instSetCounters[FACGES];
                return FACGES;

            }
            else {
                ++instSetCounters[FACGTS];
                return FACGTS;

            }
        case 0x1f:
            if (u) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (size < 0x2) {
                ++instSetCounters[FRECPSS];
                return FRECPSS;

            }
            else {
                ++instSetCounters[FRSQRTSS];
                return FRSQRTSS;

            }
        default: {
            ++instSetCounters[Unknown64];
            return Unknown64;

        }
    }
}

inline Aarch64Set aarch64::decodeNeonSc3Diff(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg) {
    if (bits(machInst, 29)) {
        ++instSetCounters[Unknown64];
        return Unknown64;

    }

    uint8_t size = bits(machInst, 23, 22);

    if (size == 0x0 || size == 0x3) {
        ++instSetCounters[Unknown64];
        return Unknown64;

    }

    uint8_t opcode = bits(machInst, 15, 12);

    IntRegIndex vd = (IntRegIndex) (uint8_t) bits(machInst, 4, 0);
    IntRegIndex vn = (IntRegIndex) (uint8_t) bits(machInst, 9, 5);
    IntRegIndex vm = (IntRegIndex) (uint8_t) bits(machInst, 20, 16);
    instReg.DReg[2] = vd;
    instReg.SReg[3] = vn;
    instReg.SReg[4] = vm;

    switch (opcode) {
        case 0x9: {
            ++instSetCounters[SQDMLAL];
            return SQDMLAL;

        }
        case 0xb: {
            ++instSetCounters[SQDMLSL];
            return SQDMLSL;

        }
        case 0xd: {
            ++instSetCounters[SQDMULL];
            return SQDMULL;

        }
        default: {
            ++instSetCounters[Unknown64];
            return Unknown64;

        }
    }
}

inline Aarch64Set aarch64::decodeNeonSc2RegMisc(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg) {
    uint8_t u = bits(machInst, 29);
    uint8_t size = bits(machInst, 23, 22);
    uint8_t opcode = bits(machInst, 16, 12);

    IntRegIndex vd = (IntRegIndex) (uint8_t) bits(machInst, 4, 0);
    IntRegIndex vn = (IntRegIndex) (uint8_t) bits(machInst, 9, 5);
    instReg.DReg[2] = vd;
    instReg.SReg[3] = vn;

    uint8_t switchVal = opcode | ((u ? 1 : 0) << 5);
    switch (switchVal) {
        case 0x03: {
            ++instSetCounters[SUQADDS];
            return SUQADDS;

        }
        case 0x07: {
            ++instSetCounters[SQABSS];
            return SQABSS;

        }
        case 0x08:
            if (size != 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[CmgtZeroDX];
                return CmgtZeroDX;

            }
        case 0x09:
            if (size != 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[CmeqZeroDX];
                return CmeqZeroDX;

            }
        case 0x0a:
            if (size != 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[CmltZeroDX];
                return CmltZeroDX;

            }
        case 0x0b:
            if (size != 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[AbsDX];
                return AbsDX;

            }
        case 0x0c:
            if (size < 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[FCMGTS];
                return FCMGTS;

            }
        case 0x0d:
            if (size < 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[FCMEQS];
                return FCMEQS;

            }
        case 0x0e:
            if (size < 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[FCMLTS];
                return FCMLTS;

            }
        case 0x14:
            if (size == 0x3) {
                {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
            else {
                switch (size) {
                    case 0x0: {
                        ++instSetCounters[SqxtnScX];
                        return SqxtnScX;

                    }
                    case 0x1: {
                        ++instSetCounters[SqxtnScX];
                        return SqxtnScX;

                    }
                    case 0x2: {
                        ++instSetCounters[SqxtnScX];
                        return SqxtnScX;

                    }
                }
            }
        case 0x1a:
            if (size < 0x2) {
                ++instSetCounters[FCVTNS];
                return FCVTNS;

            }
            else {
                ++instSetCounters[FCVTPSS];
                return FCVTPSS;

            }
        case 0x1b:
            if (size < 0x2) {
                ++instSetCounters[FCVTMSS];
                return FCVTMSS;

            }
            else {
                ++instSetCounters[FCVTZSS];
                return FCVTZSS;

            }
        case 0x1c:
            if (size < 0x2) {
                ++instSetCounters[FCVTASS];
                return FCVTASS;

            }
            else {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
        case 0x1d:
            if (size < 0x2) {
                if (size & 0x1) {
                    ++instSetCounters[ScvtfIntScDX];
                    return ScvtfIntScDX;

                }
                else {
                    ++instSetCounters[ScvtfIntScSX];
                    return ScvtfIntScSX;

                }
            }
            else {
                {
                    ++instSetCounters[FRECPES];
                    return FRECPES;

                }
            }
        case 0x1f:
            if (size < 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[FRECPXS];
                return FRECPXS;

            }
        case 0x23: {
            ++instSetCounters[USQADDS];
            return USQADDS;

        }
        case 0x27: {
            ++instSetCounters[SQNEGS];
            return SQNEGS;

        }
        case 0x28:
            if (size != 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[CMGES];
                return CMGES;

            }
        case 0x29:
            if (size != 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[CmleZeroDX];
                return CmleZeroDX;

            }
        case 0x2b:
            if (size != 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[NegDX];
                return NegDX;

            }
        case 0x2c:
            if (size < 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[FCMGES];
                return FCMGES;

            }
        case 0x2d:
            if (size < 0x2) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[FCMLES];
                return FCMLES;

            }
        case 0x32:
            if (size == 0x3) {
                {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
            else {
                switch (size) {
                    case 0x0: {
                        ++instSetCounters[SqxtunScX];
                        return SqxtunScX;

                    }
                    case 0x1: {
                        ++instSetCounters[SqxtunScX];
                        return SqxtunScX;

                    }
                    case 0x2: {
                        ++instSetCounters[SqxtunScX];
                        return SqxtunScX;

                    }
                }
            }
        case 0x34:
            if (size == 0x3) {
                {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
            else {
                switch (size) {
                    case 0x0: {
                        ++instSetCounters[UqxtnScX];
                        return UqxtnScX;

                    }
                    case 0x1: {
                        ++instSetCounters[UqxtnScX];
                        return UqxtnScX;

                    }
                    case 0x2: {
                        ++instSetCounters[UqxtnScX];
                        return UqxtnScX;

                    }
                }
            }
        case 0x36:
            if (size != 0x1) {
                {
                    ++instSetCounters[Unknown64];
                    return Unknown64;

                }
            }
            else {
                {
                    ++instSetCounters[FcvtxnScX];
                    return FcvtxnScX;

                }
            }
        case 0x3a:
            if (size < 0x2) {
                ++instSetCounters[FCVTNUS];
                return FCVTNUS;

            }
            else {
                ++instSetCounters[FCVTPUS];
                return FCVTPUS;

            }
        case 0x3b:
            if (size < 0x2) {
                ++instSetCounters[FCVTMUS];
                return FCVTMUS;

            }
            else {
                ++instSetCounters[FCVTZUS];
                return FCVTZUS;

            }
        case 0x3c:
            if (size < 0x2) {
                ++instSetCounters[FCVTAUS];
                return FCVTAUS;

            }
            else {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
        case 0x3d:
            if (size < 0x2) {
                ++instSetCounters[UCVTFS];
                return UCVTFS;

            }
            else {
                ++instSetCounters[FRSQRTES];
                return FRSQRTES;

            }
        default: {
            ++instSetCounters[Unknown64];
            return Unknown64;

        }
    }
}

inline Aarch64Set aarch64::decodeNeonScPwise(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg) {
    uint8_t u = bits(machInst, 29);
    uint8_t size = bits(machInst, 23, 22);
    uint8_t opcode = bits(machInst, 16, 12);

    IntRegIndex vd = (IntRegIndex) (uint8_t) bits(machInst, 4, 0);
    IntRegIndex vn = (IntRegIndex) (uint8_t) bits(machInst, 9, 5);
    instReg.DReg[2] = vd;
    instReg.SReg[3] = vn;

    if (!u) {
        if (opcode == 0x1b && size == 0x3) {
            ++instSetCounters[AddpScQX];
            return AddpScQX;

        }
        else {
            ++instSetCounters[Unknown64];
            return Unknown64;

        }
    }
    uint8_t switchVal = (opcode << 0) | (size << 5);

    switch (switchVal) {
        case 0x0c:
        case 0x2c: {
            ++instSetCounters[FMAXNMPS];
            return FMAXNMPS;

        }
        case 0x0d:
        case 0x2d: {
            ++instSetCounters[FADDPS];
            return FADDPS;

        }
        case 0x0f:
        case 0x2f: {
            ++instSetCounters[FMAXPS];
            return FMAXPS;

        }
        case 0x4c:
        case 0x6c: {
            ++instSetCounters[FMINNMPS];
            return FMINNMPS;

        }
        case 0x4f:
        case 0x6f: {
            ++instSetCounters[FMINPS];
            return FMINPS;

        }
        default: {
            ++instSetCounters[Unknown64];
            return Unknown64;

        }
    }
}

inline Aarch64Set aarch64::decodeNeonScCopy(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg) {
    if (bits(machInst, 14, 11) != 0 || bits(machInst, 29)) {
        ++instSetCounters[Unknown64];
        return Unknown64;

    }

    uint8_t imm5 = bits(machInst, 20, 16);

    IntRegIndex vd = (IntRegIndex) (uint8_t) bits(machInst, 4, 0);
    IntRegIndex vn = (IntRegIndex) (uint8_t) bits(machInst, 9, 5);
    instReg.DReg[2] = vd;
    instReg.SReg[3] = vn;

    {
        ++instSetCounters[DupElem];
        return DupElem;

    }
}

inline Aarch64Set aarch64::decodeNeonScIndexedElem(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg) {
    uint8_t u = bits(machInst, 29);
    uint8_t size = bits(machInst, 23, 22);
    uint8_t L = bits(machInst, 21);
    uint8_t M = bits(machInst, 20);
    uint8_t opcode = bits(machInst, 15, 12);
    uint8_t H = bits(machInst, 11);

    IntRegIndex vd = (IntRegIndex) (uint8_t) bits(machInst, 4, 0);
    IntRegIndex vn = (IntRegIndex) (uint8_t) bits(machInst, 9, 5);
    IntRegIndex vm_bf = (IntRegIndex) (uint8_t) bits(machInst, 19, 16);
    instReg.DReg[2] = vd;
    instReg.SReg[3] = vn;
    instReg.SReg[4] = vm_bf;

    uint8_t index = 0;
    uint8_t index_fp = 0;
    uint8_t vmh = 0;
    uint8_t sz_L = bits(machInst, 22, 21);

    // Index and 2nd register operand for integer instructions
    if (size == 0x1) {
        index = (H << 2) | (L << 1) | M;
        // vmh = 0;
    }
    else if (size == 0x2) {
        index = (H << 1) | L;
        vmh = M;
    }
    else if (size == 0x3) {
        index = H;
        vmh = M;
    }
    IntRegIndex vm = (IntRegIndex) (uint8_t) (vmh << 4 | vm_bf);

    // Index and 2nd register operand for FP instructions
    vmh = M;
    if ((size & 0x1) == 0) {
        index_fp = (H << 1) | L;
    }
    else if (L == 0) {
        index_fp = H;
    }
    IntRegIndex vm_fp = (IntRegIndex) (uint8_t) (vmh << 4 | vm_bf);

    if (u && opcode != 9) {
        ++instSetCounters[Unknown64];
        return Unknown64;

    }

    switch (opcode) {
        case 0x1:
            if (size < 2 || sz_L == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[FMLA];
                return FMLA;

            }
        case 0x3:
            if (size == 0x0 || size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[SQDMLAL];
                return SQDMLAL;

            }
        case 0x5:
            if (size < 2 || sz_L == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[FMLS];
                return FMLS;

            }
        case 0x7:
            if (size == 0x0 || size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[SQDMLSL];
                return SQDMLSL;

            }
        case 0x9:
            if (size < 2 || sz_L == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[FMULX];
                return FMULX;

            }
            else {
                ++instSetCounters[FMUL];
                return FMUL;

            }
        case 0xb:
            if (size == 0x0 || size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[SQDMULL];
                return SQDMULL;

            }
        case 0xc:
            if (size == 0x0 || size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[SQDMULH];
                return SQDMULH;

            }
        case 0xd:
            if (size == 0x0 || size == 0x3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            else {
                ++instSetCounters[SQRDMULH];
                return SQRDMULH;

            }
        default: {
            ++instSetCounters[Unknown64];
            return Unknown64;

        }
    }
}

inline Aarch64Set aarch64::decodeNeonScShiftByImm(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg) {
    bool u = bits(machInst, 29);
    uint8_t immh = bits(machInst, 22, 19);
    uint8_t immb = bits(machInst, 18, 16);
    uint8_t opcode = bits(machInst, 15, 11);

    IntRegIndex vd = (IntRegIndex) (uint8_t) bits(machInst, 4, 0);
    IntRegIndex vn = (IntRegIndex) (uint8_t) bits(machInst, 9, 5);
    instReg.DReg[2] = vd;
    instReg.SReg[3] = vn;

    uint8_t size = findMsbSet(immh);
    int shiftAmt = (8 << (size + 1)) - ((immh << 3) | immb);

    uint8_t immh3 = bits(machInst, 22);

    if (immh == 0x0) {
        ++instSetCounters[Unknown64];
        return Unknown64;

    }

    switch (opcode) {
        case 0x00:
            if (!immh3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UshrDX];
                return UshrDX;

            }
            else {
                ++instSetCounters[SshrDX];
                return SshrDX;

            }
        case 0x02:
            if (!immh3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UsraDX];
                return UsraDX;

            }
            else {
                ++instSetCounters[SsraDX];
                return SsraDX;

            }
        case 0x04:
            if (!immh3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UrshrDX];
                return UrshrDX;

            }
            else {
                ++instSetCounters[SrshrDX];
                return SrshrDX;

            }
        case 0x06:
            if (!immh3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UrsraDX];
                return UrsraDX;

            }
            else {
                ++instSetCounters[SrsraDX];
                return SrsraDX;

            }
        case 0x08:
            if (!immh3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[SriDX];
                return SriDX;

            }
            else {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
        case 0x0a:
            if (!immh3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            shiftAmt = ((immh << 3) | immb) - (8 << size);
            if (u) {
                ++instSetCounters[SliDX];
                return SliDX;
            }
            else {
                ++instSetCounters[ShlDX];
                return ShlDX;
            }
        case 0x0c:
            if (u) {
                shiftAmt = ((immh << 3) | immb) - (8 << size);
                ++instSetCounters[SHLS];
                return SHLS;
            }
            else {
                ++instSetCounters[Unknown64];
                return Unknown64;
            }
        case 0x0e:
            shiftAmt = ((immh << 3) | immb) - (8 << size);
            if (u) {
                ++instSetCounters[UQSHLS];
                return UQSHLS;
            }
            else {
                ++instSetCounters[SQSHLS];
                return SQSHLS;
            }
        case 0x10:
            if (!u || immh3) {
                ++instSetCounters[Unknown64];
                return Unknown64;
            }
            {
                ++instSetCounters[SQSHRUNS];
                return SQSHRUNS;
            }
        case 0x11:
            if (!u || immh3) {
                ++instSetCounters[Unknown64];
                return Unknown64;
            }
            {
                ++instSetCounters[SQRSHRUNS];
                return SQRSHRUNS;
            }
        case 0x12:
            if (immh3) {
                ++instSetCounters[Unknown64];
                return Unknown64;
            }
            if (u) {
                ++instSetCounters[UQSHRNS];
                return UQSHRNS;
            }
            else {
                ++instSetCounters[SQSHRNS];
                return SQSHRNS;
            }
        case 0x13:
            if (immh3) {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
            if (u) {
                ++instSetCounters[UQRSHRNS];
                return UQRSHRNS;
            }
            else {
                ++instSetCounters[SQRSHRNS];
                return SQRSHRNS;
            }
        case 0x1c:
            if (immh < 0x4) {
                ++instSetCounters[Unknown64];
                return Unknown64;
            }
            if (u) {
                ++instSetCounters[UCVTFS];
                return UCVTFS;
            }
            else {
                if (size & 0x1) {
                    ++instSetCounters[ScvtfFixedScDX];
                    return ScvtfFixedScDX;
                }
                else {
                    ++instSetCounters[ScvtfFixedScSX];
                    return ScvtfFixedScSX;
                }
            }
        case 0x1f:
            if (immh < 0x4) {
                ++instSetCounters[Unknown64];
                return Unknown64;
            }
            if (u) {
                ++instSetCounters[FCVTZUS];
                return FCVTZUS;
            }
            else {
                ++instSetCounters[FCVTZSS];
                return FCVTZSS;
            }
        default: {
            ++instSetCounters[Unknown64];
            return Unknown64;
        }
    }
}

// remain problem, only appear in loadstore inst    [modified by pz]problem solved
inline Aarch64Set aarch64::decodeNeonMem(const ExtMachInst machInst, int *instSetCounters, InstReg &instReg) {
    uint8_t dataSize = bits(machInst, 30) ? 128 : 64;
    bool multiple = bits(machInst, 24, 23) < 0x2;
    bool load = bits(machInst, 22);
    uint8_t numStructElems = 0;
    uint8_t numRegs = 0;

    if (multiple) {  // AdvSIMD load/store multiple structures
        uint8_t opcode = bits(machInst, 15, 12);
        uint8_t eSize = bits(machInst, 11, 10);
        bool wb = !(bits(machInst, 20, 16) == 0x0 && !bits(machInst, 23));

        switch (opcode) {
            case 0x0:  // LD/ST4 (4 regs)
                numStructElems = 4;
                numRegs = 4;
                break;
            case 0x2:  // LD/ST1 (4 regs)
                numStructElems = 1;
                numRegs = 4;
                break;
            case 0x4:  // LD/ST3 (3 regs)
                numStructElems = 3;
                numRegs = 3;
                break;
            case 0x6:  // LD/ST1 (3 regs)
                numStructElems = 1;
                numRegs = 3;
                break;
            case 0x7:  // LD/ST1 (1 reg)
                numStructElems = 1;
                numRegs = 1;
                break;
            case 0x8:  // LD/ST2 (2 regs)
                numStructElems = 2;
                numRegs = 2;
                break;
            case 0xa:  // LD/ST1 (2 regs)
                numStructElems = 1;
                numRegs = 2;
                break;
            default: {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
        }
        uint8_t rt = (uint8_t)bits(machInst, 4, 0);
        IntRegIndex rn = (IntRegIndex) (uint8_t) bits(machInst, 9, 5);
        IntRegIndex rm = (IntRegIndex) (uint8_t) bits(machInst, 20, 16);
        /* this is general-purpose register */
        instReg.SReg[0] = rn;
        //instReg.SReg[4] = rm;
        if (bits(machInst, 23) == 1 && (rm != 31))      //[added by pz] 用于前/后索引寻址方式，此时有多个SReg
            instReg.SReg[1] = rm;
        if (load) {
            {
                ++instSetCounters[VldMult64];
                //[modified by pz] 存在多个SIMD&FP寄存器
                for(int i = 2; i < (2 + numRegs); i++)      
                {
                    IntRegIndex vd = (IntRegIndex)rt;
                    instReg.DReg[i] = vd;
                    rt++;
                }
                return VldMult64;
            }
        }
        else {
            {
                ++instSetCounters[VstMult64];
                for (int i = 3; i < (3 + numRegs); i++)
                {
                    IntRegIndex vd = (IntRegIndex)rt;
                    instReg.SReg[i] = vd;
                    rt++;
                }
                return VstMult64;
            }
        }
    }
    else {  // AdvSIMD load/store single structure
        uint8_t scale = bits(machInst, 15, 14);
        uint8_t numStructElems = (((uint8_t) bits(machInst, 13) << 1) |
                                  (uint8_t) bits(machInst, 21)) + 1;
        uint8_t index = 0;
        bool wb = !(bits(machInst, 20, 16) == 0x0 && !bits(machInst, 23));
        bool replicate = false;

        /*      [deleted by pz] 以后可能会有用？
        switch (scale) {
            case 0x0:
                index = ((uint8_t) bits(machInst, 30) << 3) |
                        ((uint8_t) bits(machInst, 12) << 2) |
                        (uint8_t) bits(machInst, 11, 10);
                break;
            case 0x1:
                index = ((uint8_t) bits(machInst, 30) << 2) |
                        ((uint8_t) bits(machInst, 12) << 1) |
                        (uint8_t) bits(machInst, 11);
                break;
            case 0x2:
                if (bits(machInst, 10) == 0x0) {
                    index = ((uint8_t) bits(machInst, 30) << 1) |
                            bits(machInst, 12);
                }
                else {
                    index = (uint8_t) bits(machInst, 30);
                    scale = 0x3;
                }
                break;
            case 0x3:
                scale = bits(machInst, 11, 10);
                replicate = true;
                break;
            default: {
                ++instSetCounters[Unknown64];
                return Unknown64;

            }
        }
        */
        uint8_t eSize = scale;

        uint8_t rt = (uint8_t) bits(machInst, 4, 0);
        IntRegIndex rn = (IntRegIndex) (uint8_t) bits(machInst, 9, 5);
        IntRegIndex rm = (IntRegIndex) (uint8_t) bits(machInst, 20, 16);
        /* this is general purpose register */
        instReg.SReg[0] = rn;
        if (bits(machInst, 23) == 1 && (rm != 31))      //[added by pz]
            instReg.SReg[1] = rm;

        if (load) {
            {
                ++instSetCounters[VldSingle64];
                 //[modified by pz]存在多个SIMD&FP寄存器
                for (int i = 2; i < (2 + numStructElems); i++)     
                {
                    IntRegIndex vd = (IntRegIndex)rt;
                    instReg.DReg[i] = vd;
                    rt++;
                }
                return VldSingle64;
            }
        }
        else {
            {
                ++instSetCounters[VstSingle64];
                for (int i = 3; i < (3 + numStructElems); i++)
                {
                    IntRegIndex vd = (IntRegIndex)rt;
                    instReg.SReg[i] = vd;
                    rt++;
                }
                return VstSingle64;
            }
        }
    }
}
