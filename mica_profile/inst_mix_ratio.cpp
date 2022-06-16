//
// This is to calculate the instruction's mix ratio
// instMix: the items that we are tracing
// instA64Counters: the number of instruction sets that are used
// instA32Counters: arm A32 instruction set counters
//
// Created by Fu Wenjie
//

#include "global_def.h"
#include "inst_mix_ratio.h"
#include "decode_inst_set.h"

// changed by shen 11/13
void InstMixRatio::instMixRatio(const int *instA64Counters, const int *instA32Counters){ // by shen 10/24  //[modified by pz]
	instMix.unKnownNum = instA32Counters[aarch32::Unknown32] + instA64Counters[aarch64::Unknown64];

	/* Arch32 */
	/* int ALU inst latency is 1-2 */
	for (int i = aarch32::AdcImm; i <= aarch32::MovsRegPclr; i++)												//20-167
		instMix.intAluNum += instA32Counters[i];
	/* int Multi-Alu latency is 3-4 */
	for (int i = aarch32::QasxReg; i <= aarch32::Umull; i++)													//168-236
		instMix.intMultiDivNum += instA32Counters[i];

	/* int Mul latency is 5*/
	//instMix.intMulNum += instA64Counters[Smulh64] + instA64Counters[Umulh64];
	/* Fp&SIMD Alu inst latency is 3-5 */
	for (int i = aarch32::VabsD; i <= aarch32::VmulsDFp; i++)													//530-605
		instMix.fpNeonAluNum += instA32Counters[i];
 //	for (int i = aarch32::VmovD; i <= aarch32::Vmov2Reg2Core; i++)	//540-595 [deleted by pz]
 //		instMix.fpNeonAluNum += instA32Counters[i];
	for (int i = aarch32::NVabs; i <= aarch32::VpminQFp; i++)													//668-847
		instMix.fpNeonAluNum += instA32Counters[i];
	/* Fp&SIMD Divide insts latency is 7-8 and 6*/
	for (int i = aarch32::VdivD; i <= aarch32::VmovRegCoreW; i++)												//620-631
		instMix.fpNeonDivNum += instA32Counters[i];
	instMix.fpNeonDivNum += instA32Counters[aarch32::VqdmulhsQ] + instA32Counters[aarch32::VqrdmulhsQ];			//848-849

	/* Fp&SIMD Multiply insts latency is 8-10 */
	for (int i = aarch32::VmlasDFp; i <= aarch32::VnmlsS; i++)													//632-659
		instMix.fpNeonMulNum += instA32Counters[i];
	for (int i = aarch32::NVdup; i <= aarch32::VrsqrtsQFp; i++)													//850,869
		instMix.fpNeonMulNum += instA32Counters[i];

	instMix.serialInstNum += instA32Counters[aarch32::Dsb] + instA32Counters[aarch32::Dmb] + instA32Counters[aarch32::Isb];		//877-879

	for (int i = aarch32::B; i <= aarch32::Cbz; i++)/* include expection generation and system instructions */					//1-7
		instMix.branchInstNum += instA32Counters[i];

	for (int i = aarch32::LdmStmLd; i <= aarch32::Pli_radd; i++)												//237-424
		instMix.loadInstNum += instA32Counters[i];
	for (int i = aarch32::Vldr_d; i <= aarch32::Vldr_us; i++)													//660-663,870
		instMix.loadInstNum += instA32Counters[i];
	instMix.loadInstNum += instA32Counters[aarch32::VLdmStmLd];

	for (int i = aarch32::Str; i <= aarch32::Strt; i++)															//425-529
		instMix.storeInstNum += instA32Counters[i];
	for (int i = aarch32::Vstr_d; i <= aarch32::Vstr_us; i++)													//664-667,873
		instMix.storeInstNum += instA32Counters[i];
	instMix.storeInstNum += instA32Counters[aarch32::VLdmStmSt];

	for (int i = aarch32::B; i <= aarch32::Strt; i++)
		instMix.intInstNum += instA32Counters[i];

	for (int i = aarch32::VabsD; i <= aarch32::Vstr_us; i++)	//530-667
		instMix.fpInstNum += instA32Counters[i];

	for (int i = aarch32::NVabs; i <= aarch32::VLdmStmSt; i++) // exclude fp instrution by shen 10/24			//668-873
		instMix.SIMDInstNum += instA32Counters[i];


	/* Arch64 */
	/* int ALU inst latency is 1-2 */																						//411-776,411-729(15906),749-758(5770)
	for (int i = aarch64::AddXImm; i <= aarch64::Bfm64; i++)																//1-56--
		instMix.intAluNum += instA64Counters[i];
	/* int Multi-Alu latency is 3-4 */
	for (int i = aarch64::Madd64; i <= aarch64::Sdiv64; i++)																//57-64--
		instMix.intMultiDivNum += instA64Counters[i];
	/* int Mul latency is 5*/
	instMix.intMulNum += instA64Counters[aarch64::Smulh64] + instA64Counters[aarch64::Umulh64];								//65-66--
	/* Fp&SIMD Alu inst latency is 3-5 */
	for (int i = aarch64::FAbsS; i <= aarch64::FRIntID; i++)																//271-334--
		instMix.fpNeonAluNum += instA64Counters[i];

	for (int i = aarch64::SABD; i <=aarch64::UmovWX; i++)																	//411-729-
		instMix.fpNeonAluNum += instA64Counters[i];
	/* Fp&SIMD Divide insts latency is 7 */
	for (int i = aarch64::FDivS; i <= aarch64::FSqrtD; i++)																	//335-338--
		instMix.fpNeonDivNum += instA64Counters[i];
	instMix.fpNeonDivNum += instA64Counters[aarch64::FDIV] + instA64Counters[aarch64::FSQRT];

	/* Fp&SIMD Multiply insts latency is 9 */
	for (int i = aarch64::FMAddS; i <= aarch64::FNMSubD; i++)																//339-346--
		instMix.fpNeonMulNum += instA64Counters[i];

	for (int i = aarch64::DupGprWDX; i <= aarch64::FMLSS; i++)																//749-758	749-758
		instMix.fpNeonMulNum += instA64Counters[i];

	instMix.serialInstNum += instA64Counters[aarch64::Dsb64] + instA64Counters[aarch64::Dmb64] + instA64Counters[aarch64::Isb64];	//86-88(2)

	for (int i = aarch64::B64; i <= aarch64::Eret64; i++)/* include expection generation and system instructions */			//67-97--
		instMix.branchInstNum += instA64Counters[i];

	for (int i = aarch64::LDXRB64; i <= aarch64::BigFpMemPreLd; i++)														//98-204--
		instMix.loadInstNum += instA64Counters[i];
	instMix.loadInstNum += instA64Counters[aarch64::VldMult64] + instA64Counters[aarch64::VldSingle64];						//730-731(2702)

	for (int i = aarch64::STXRB64; i <= aarch64::BigFpMemPreSt; i++)														//205-270--
		instMix.storeInstNum += instA64Counters[i];
	instMix.storeInstNum += instA64Counters[aarch64::VstMult64] + instA64Counters[aarch64::VstSingle64];					//732-733(0)

	for (int i = aarch64::AddXImm; i <= aarch64::Umulh64; i++)
		instMix.intInstNum += instA64Counters[i];

	for (int i = aarch64::FAbsS; i <= aarch64::FcvtFpUIntXDZ; i++)	//271-410
		instMix.fpInstNum += instA64Counters[i];

	for (int i = aarch64::SABD; i <= aarch64::Tbx4QX; i++) // exclude fp instrution by shen 10/24								411-776 411-729
		instMix.SIMDInstNum += instA64Counters[i];
}
