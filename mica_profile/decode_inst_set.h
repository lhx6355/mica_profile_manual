//
// Created by shenshan on 16-9-22.
//

#ifndef DECODE_INST_SET_H
#define DECODE_INST_SET_H

enum ConditionCode {
	COND_EQ = 0,
	COND_NE, //  1
	COND_CS, //  2
	COND_CC, //  3
	COND_MI, //  4
	COND_PL, //  5
	COND_VS, //  6
	COND_VC, //  7
	COND_HI, //  8
	COND_LS, //  9
	COND_GE, // 10
	COND_LT, // 11
	COND_GT, // 12
	COND_LE, // 13
	COND_AL, // 14
	COND_UC  // 15
};

enum OperatingMode {
	MODE_EL0T = 0x0,
	MODE_EL1T = 0x4,
	MODE_EL1H = 0x5,
	MODE_EL2T = 0x8,
	MODE_EL2H = 0x9,
	MODE_EL3T = 0xC,
	MODE_EL3H = 0xD,
	MODE_USER = 16,
	MODE_FIQ = 17,
	MODE_IRQ = 18,
	MODE_SVC = 19,
	MODE_MON = 22,
	MODE_ABORT = 23,
	MODE_HYP = 26,
	MODE_UNDEFINED = 27,
	MODE_SYSTEM = 31,
	MODE_MAXMODE = MODE_SYSTEM
};

// Extension types for ARM instructions
enum ArmExtendType {
	UXTB = 0,
	UXTH = 1,
	UXTW = 2,
	UXTX = 3,
	SXTB = 4,
	SXTH = 5,
	SXTW = 6,
	SXTX = 7
};

enum ArmShiftType {
	LSL = 0,
	LSR,
	ASR,
	ROR
};

static inline bool badMode32(OperatingMode mode)
{
	switch (mode) {
	case MODE_USER:
	case MODE_FIQ:
	case MODE_IRQ:
	case MODE_SVC:
	case MODE_MON:
	case MODE_ABORT:
	case MODE_HYP:
	case MODE_UNDEFINED:
	case MODE_SYSTEM:
		return false;
	default:
		return true;
	}
}

static inline uint32_t msrMrs64IssBuild(bool isRead, uint32_t op0, uint32_t op1, uint32_t crn, uint32_t crm, uint32_t op2, IntRegIndex rt) {
	return isRead |
		(crm << 1) |
		(rt << 5) |
		(crn << 10) |
		(op1 << 14) |
		(op2 << 17) |
		(op0 << 20);
}

/* Generate a 64-bit mask of 'nbits' 1s, right justified.
*/
inline uint64_t mask(int nbits)
{
	return (nbits == 64) ? (uint64_t)-1LL : (1ULL << nbits) - 1;
}

/**
* Extract the bitfield from position 'first' to 'last' (inclusive)
* from 'val' and right justify it.  MSB is numbered 63, LSB is 0.
*/
template <class T>
inline T bits(T val, int first, int last)
{
	int nbits = first - last + 1;
	return (val >> last) & mask(nbits);
}

/**
* Extract the bit from this position from 'val' and right justify it.
*/
template <class T>
inline T bits(T val, int bit)
{
	return bits(val, bit, bit);
}

static inline IntRegIndex makeSP(IntRegIndex reg)
{
	if (reg == INTREG_X31)
		reg = INTREG_SPX;
	return reg;
}

static inline IntRegIndex makeZero(IntRegIndex reg)
{
	if (reg == INTREG_X31)
		reg = INTREG_ZERO;
	return reg;
}

inline int findLsbSet(uint64_t val) {
	int lsb = 0;
	if (!val)
		return sizeof(val) * 8;
	if (!bits(val, 31, 0)) { lsb += 32; val >>= 32; }
	if (!bits(val, 15, 0)) { lsb += 16; val >>= 16; }
	if (!bits(val, 7, 0))  { lsb += 8;  val >>= 8; }
	if (!bits(val, 3, 0))  { lsb += 4;  val >>= 4; }
	if (!bits(val, 1, 0))  { lsb += 2;  val >>= 2; }
	if (!bits(val, 0, 0))  { lsb += 1; }
	return lsb;
}

inline int findMsbSet(uint64_t val) {
	int msb = 0;
	if (!val)
		return 0;
	if (bits(val, 63, 32)) { msb += 32; val >>= 32; }
	if (bits(val, 31, 16)) { msb += 16; val >>= 16; }
	if (bits(val, 15, 8))  { msb += 8;  val >>= 8; }
	if (bits(val, 7, 4))   { msb += 4;  val >>= 4; }
	if (bits(val, 3, 2))   { msb += 2;  val >>= 2; }
	if (bits(val, 1, 1))   { msb += 1; }
	return msb;
}

/**
* Sign-extend an N-bit value to 64 bits.
*/
template <int N>
inline uint64_t sext(uint64_t val)
{
	int sign_bit = bits(val, N - 1, N - 1);
	return sign_bit ? (val | ~mask(N)) : val;
}

#endif //DECODE_INST_SET_H
