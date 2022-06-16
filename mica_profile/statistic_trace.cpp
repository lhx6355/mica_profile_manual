//
// This is the main function to realize source trace project
// instTrace: the input data that need to be traced
// size: the size of the input data (number of instructions it has)
// path: the path of the output file that used to be analysed
//
// Created by Shen shan
//

#include "statistic_trace.h"
//#define DEBUG

//#define PRINTUNK

#ifdef DEBUG
#include <time.h>
#endif

void statisticTrace(InstTrace* instTrace, const int& size, const char* file_fea_path, const char* file_mica_path) {
	int instA64Counters[INST_A64_SIZE] = { 0 };
	int instA32Counters[INST_A32_SIZE] = { 0 };
	int latencyA64[INST_A64_SIZE] = { 0 };
	int latencyA32[INST_A32_SIZE] = { 0 };

	/* initialize cycles of instruction latency */
	initLatency(latencyA32, latencyA64);

	/* 7 dimensions */
	CriticalPath critical;	
	BranchBehavior branch;	
	DataLocality data;		
	RegDepend reg;			
	InstLocality inst;		
	SerialInstDist serial;	
	InstMixRatio mixRatio;	

	/* other info */
	uint32_t armInstNum = 0;
	uint32_t thumbInstNum = 0;
	uint32_t jazelleInstNum = 0;
	uint32_t aarch64InstNum = 0;

#ifdef DEBUG
	long start = clock();
#endif

#ifdef PRINTUNK
	ofstream fout(path, ios::out); // to print unknown shen 11/22
#endif
	for (int i = 0; i < size; ++i) {
		InstReg instReg;
		InstType instType;
		MachInst machInst = instTrace[i].opcode;

		aarch64::Aarch64Set instA64Index;
		aarch32::Aarch32Set instA32Index;

		switch (instTrace[i].inst_iset) {			// [lhx]  根据指令的类型进行解码
		case ISET_ARM: {
			++armInstNum;
			instA32Index = aarch32::decodeA32(machInst, instA32Counters, instReg, instType); // [lhx]  返回指令的类型编码(枚举)
			/* handle the unknown instruction */
			if (instA32Index == aarch32::Unknown32) {
				instType.isUnKnown = true;
				instType.isBranch = false;
				instType.isLoad = false;
				instType.isStore = false;
				instType.isSerial = false;
				instType.isFloat = false;
				instType.isMISC = false;
				instType.isCc = false;
			}
			instReg.delay = latencyA32[instA32Index];
			//critical path
			/* if the inst is unknown, do not need to calculate critial path */
			if (!instType.isUnKnown) {
				critical.calCriticalPath(instReg);
			}

			//register dependency
			reg.regDependLength(instReg, instType, i);

			// a simple test
			//std::cout << "code " << hex << machInst << "  instA32Index is ";
			//cout.unsetf(ios::hex);
			//std::cout << instA32Index << endl;

			break;
		}
		case ISET_THUMB: {
			++thumbInstNum;
			instA32Index = aarch32::decodeT32(machInst, instA32Counters, instReg, instType);
			/* handle the unknown instruction */
 			if (instA32Index == aarch32::Unknown32) {
				instType.isUnKnown = true;
				instType.isBranch = false;
				instType.isLoad = false;
				instType.isStore = false;
				instType.isSerial = false;
				instType.isFloat = false;
				instType.isMISC = false;
				instType.isCc = false;
			}
			instReg.delay = latencyA32[instA32Index];

			//critical path
			/* if the inst is unknown, do not need to calculate critial path */
			if (!instType.isUnKnown) {
				critical.calCriticalPath(instReg);
			}

			//register dependency
			reg.regDependLength(instReg, instType, i);

			// a simple test
			//std::cout << "code " <<hex<< machInst << "  instA32Index is " ;
			//cout.unsetf(ios::hex);
			//std::cout << instA32Index << endl;


			break;
		}
		case ISET_JAZELLE: {
			++jazelleInstNum;
			instType.isUnKnown = true;
			break;
		}
		case ISET_AARCH64: {
			++aarch64InstNum;
			instA64Index = aarch64::decodeA64(machInst, instA64Counters, instReg, instType);
			/* handle the unknown instruction */
			if (instA64Index == aarch64::Unknown64) {
				instType.isUnKnown = true;
				instType.isBranch = false;
				instType.isLoad = false;
				instType.isStore = false;
				instType.isSerial = false;
				instType.isFloat = false;
				instType.isMISC = false;
				instType.isCc = false;
			}
			instReg.delay = latencyA64[instA64Index];
			//critical path
			/* if the inst is unknown, do not need to calculate critial path */
			if (!instType.isUnKnown) {
				critical.calCriticalPath(instReg);
			}

			//register dependency
			reg.regDependLength(instReg, instType, i);
			break;
		}
		default:
			break;


		}

		if (instType.isLoad)			//[pz]用于修正输入的load/store_paddr
			instTrace[i].store_paddr = 0;		
		else if(instType.isStore)
			instTrace[i].load_paddr = 0;
		else {
			instTrace[i].store_paddr = 0;
			instTrace[i].load_paddr = 0;
		}

#ifdef PRINTUNK
		// add to print unknown opcode by shen 11/22
		if (instType.isUnKnown) {
			fout << hex << "iset is " << (int)instTrace[i].inst_iset << " opcode is " << machInst << endl;
		}
#endif

		//instruction locality
		inst.instLocality(instTrace[i]);

		//data locality
		data.dataLocality(instTrace[i], instType);

		//branch behavior
		if (i < size - 1)
			branch.calBranchBehavior(instTrace[i], instTrace[i + 1], instType.isBranch);

		//serial instruction distribution
		serial.serialInstDist(instType.isSerial);


		//a simple test
		/* std::cout << instTrace[i].inst_paddr << endl;
		std::cout << instTrace[i].load_paddr << endl;
		std::cout << instTrace[i].load_vaddr << endl;
		std::cout << instTrace[i].store_paddr << endl;
		std::cout << instTrace[i].store_vaddr << endl;*/
	}
#ifdef PRINTUNK
	fout.close(); // to print unknown by shen 11/22
#endif
	//mix ratio of the instruction
	mixRatio.instMixRatio(instA64Counters, instA32Counters);

	printInfo(file_fea_path, file_mica_path, mixRatio.getInstMix(), critical.getCritPathLen(), critical.getSize(), reg.getRegDepLen(), reg.getTrace(), \
		reg.getSize(), inst.getFetchReuseDist(), inst.getFetchAddrDist(), inst.getSize(), data.getLdGlobalTemp(), \
		data.getLdGlobalSpac(), data.getLdLocalTemp(), data.getLdLocalSpac(), data.getStGlobalTemp(), data.getStGlobalSpac(), \
		data.getStLocalTemp(), data.getStLocalSpac(), data.getSize(), branch.getBraJumDist(), branch.getBraBasBlock(), \
		branch.getBranch(), branch.getSize(), serial.getSerialBlock(), serial.getSize(), armInstNum, thumbInstNum, jazelleInstNum, aarch64InstNum);

	/*
	printInfo(path, name, mixRatio.getInstMix(), critical.getCritPathLen(), critical.getSize(), reg.getRegDepLen(), reg.getTrace(), \
		reg.getSize(), inst.getFetchReuseDist(), inst.getFetchAddrDist(), inst.getSize(), data.getLdGlobalTemp(), \
		data.getLdGlobalSpac(), data.getLdLocalTemp(), data.getLdLocalSpac(), data.getStGlobalTemp(), data.getStGlobalSpac(), \
		data.getStLocalTemp(), data.getStLocalSpac(), data.getSize(), branch.getBraJumDist(), branch.getBraBasBlock(), \
		branch.getBranch(), branch.getSize(), serial.getSerialBlock(), serial.getSize(), armInstNum, thumbInstNum, jazelleInstNum, aarch64InstNum);*/

#ifdef DEBUG
	long end = clock();
	long ti = end - start;
	//cout << "Total run time = " << end - start << " ms" << endl;
#endif
	return;
}

void initLatency(int* latencyA32Ptr, int* latencyA64Ptr) {
	/* initial arch32 instruction's latency */
	for (int i = aarch32::B; i <= aarch32::MovRegCc; ++i)
		latencyA32Ptr[i] = 1;
	for (int i = aarch32::AdcImmCc; i <= aarch32::UqsaxReg; ++i)
		latencyA32Ptr[i] = 2;
	for (int i = aarch32::Mla; i <= aarch32::Usada8; ++i)
		latencyA32Ptr[i] = 3;
	for (int i = aarch32::Sxtab16; i <= aarch32::Pli_radd; ++i)
		latencyA32Ptr[i] = 4;
	for (int i = aarch32::Str; i <= aarch32::Strt; ++i)
		latencyA32Ptr[i] = 1;
	for (int i = aarch32::VabsD; i <= aarch32::VmovRegS; ++i)
		latencyA32Ptr[i] = 3;
	for (int i = aarch32::NVcvt2sfxD; i <= aarch32::Vldr_us; ++i)
		latencyA32Ptr[i] = 5;
	for (int i = aarch32::VcmpeZeroD; i <= aarch32::VnmulS; ++i)
		latencyA32Ptr[i] = 6;
	for (int i = aarch32::VdivD; i <= aarch32::VsqrtS; ++i)
		latencyA32Ptr[i] = 7;
	for (int i = aarch32::VmovCoreRegB; i <= aarch32::VmovRegCoreW; ++i)
		latencyA32Ptr[i] = 8;
	for (int i = aarch32::VmlasDFp; i <= aarch32::VmlasDFp; ++i)
		latencyA32Ptr[i] = 9;
	for (int i = aarch32::VfmaD; i <= aarch32::VnmlsS; ++i)
		latencyA32Ptr[i] = 10;
	for (int i = aarch32::Vldr_d; i <= aarch32::Vldr_us; ++i)
		latencyA32Ptr[i] = 5;
	for (int i = aarch32::Vstr_d; i <= aarch32::Vstr_us; ++i)
		latencyA32Ptr[i] = 1;
	for (int i = aarch32::NVabs; i <= aarch32::VbslQ; ++i)
		latencyA32Ptr[i] = 3;
	for (int i = aarch32::NVqmovn; i <= aarch32::Vrshl; ++i)
		latencyA32Ptr[i] = 4;
	for (int i = aarch32::NVrecpeD; i <= aarch32::VpminQFp; ++i)
		latencyA32Ptr[i] = 5;

	latencyA32Ptr[aarch32::VqdmulhsQ] = 6;
	latencyA32Ptr[aarch32::VqrdmulhsQ] = 6;

	for (int i = aarch32::NVdup; i <= aarch32::NVdupQGpr; ++i)
		latencyA32Ptr[i] = 8;
	for (int i = aarch32::NVfmaDF; i <= aarch32::NVrsqrteQFp; ++i)
		latencyA32Ptr[i] = 9;
	//ASIMD Load Instructions
	// latency = 8
	for (int i = aarch32::VLdmStmLd; i <= aarch32::VldMult; ++i)
		latencyA32Ptr[i] = 8;
	//ASIMD Store Instructions
	// latency = 3
	for (int i = aarch32::VLdmStmSt; i <= aarch32::VstMult; ++i)
		latencyA32Ptr[i] = 3;

	latencyA32Ptr[aarch32::VrecpsQFp] = 10;
	latencyA32Ptr[aarch32::VrsqrtsQFp] = 10;

	/* initial arch64 instruction's latency */
	for (int i = aarch64::AddXImm; i <= aarch64::Cls64; ++i)
		latencyA64Ptr[i] = 1;
	for (int i = aarch64::OrrXSReg; i <= aarch64::Bfm64; ++i)
		latencyA64Ptr[i] = 2;
	for (int i = aarch64::Madd64; i <= aarch64::Umsubl64; ++i)
		latencyA64Ptr[i] = 3;
	for (int i = aarch64::Udiv64; i <= aarch64::Sdiv64; ++i)
		latencyA64Ptr[i] = 4;
	for (int i = aarch64::Smulh64; i <= aarch64::Umulh64; ++i)
		latencyA64Ptr[i] = 6;
	for (int i = aarch64::B64; i <= aarch64::Tbnz64; ++i)
		latencyA64Ptr[i] = 1;
	for (int i = aarch64::Blr64; i <= aarch64::Eret64; ++i)
		latencyA64Ptr[i] = 2;
	for (int i = aarch64::LDXRB64; i <= aarch64::BigFpMemPreLd; ++i)
		latencyA64Ptr[i] = 4;
	for (int i = aarch64::STXRB64; i <= aarch64::BigFpMemPreSt; ++i)
		latencyA64Ptr[i] = 1;
	for (int i = aarch64::FAbsS; i <= aarch64::FmovRegD; ++i)
		latencyA64Ptr[i] = 3;
	for (int i = aarch64::FAddS; i <= aarch64::FRIntID; ++i)
		latencyA64Ptr[i] = 5;
	for (int i = aarch64::FDivS; i <= aarch64::FSqrtD; ++i)
		latencyA64Ptr[i] = 7;
	for (int i = aarch64::FMAddS; i <= aarch64::FNMSubD; ++i)
		latencyA64Ptr[i] = 7;
	for (int i = aarch64::FcvtWSIntFpS; i <= aarch64::FcvtFpUIntXDZ; ++i)
		latencyA64Ptr[i] = 10;
	for (int i = aarch64::SABD; i <= aarch64::PmullX; ++i)
		latencyA64Ptr[i] = 3;
	for (int i = aarch64::SABA; i <= aarch64::UqxtnScX; ++i)
		latencyA64Ptr[i] = 4;
	for (int i = aarch64::MUL; i <= aarch64::VstSingle64; ++i)
		latencyA64Ptr[i] = 5;
	for (int i = aarch64::Tbl1QX; i <= aarch64::UmovXX; ++i)
		latencyA64Ptr[i] = 6;
	for (int i = aarch64::FDIV; i <= aarch64::FSQRT; ++i)
		latencyA64Ptr[i] = 7;
	for (int i = aarch64::Fcvtl2X; i <= aarch64::InsGprXX; ++i)
		latencyA64Ptr[i] = 8;
	for (int i = aarch64::FMLA; i <= aarch64::Tbx3DX; ++i)
		latencyA64Ptr[i] = 9;
	for (int i = aarch64::FmaxnmvQX; i <= aarch64::FminvQX; ++i)
		latencyA64Ptr[i] = 10;
	for (int i = aarch64::Tbl3QX; i <= aarch64::Tbx4DX; ++i)
		latencyA64Ptr[i] = 12;
	for (int i = aarch64::Tbl4QX; i <= aarch64::Tbx4QX; ++i)
		latencyA64Ptr[i] = 15;
}


/*void printInfo(const char *path,const char *name, const InstMix & mix, const int *critical, const int &critSize, \
	const int *regDepLen, const Trace &trace, const int &regDepSize, \
	const int *fetchReuseDist, const int *fetchAddrDist, const int &instLocaSize, \
	const int *ldGlobaltemp, const int *ldGlobalspac, const int *ldLocaltemp, \
	const int *ldLocalspac, const int *stGlobaltemp, const int *stGlobalspac, \
	const int *stLocaltemp, const int *stLocalspac, const int & dataLocaSize, \
	const int *branchJumpDistance, const int *branchBasicBlockSize, \
	const Branch &branch, const int &branchSize, const int *serialBlockSize, \
	const int &serialSize, const uint32_t armInstNum, const uint32_t thumbInstNum, \
	const uint32_t jazelleInstNum, const uint32_t aarch64InstNum) */

void printInfo(const char* file_fea_path, const char* file_mica_path, \
	const InstMix& mix, const int* critical, const int& critSize, \
	const int* regDepLen, const Trace& trace, const int& regDepSize, \
	const int* fetchReuseDist, const int* fetchAddrDist, const int& instLocaSize, \
	const int* ldGlobaltemp, const int* ldGlobalspac, const int* ldLocaltemp, \
	const int* ldLocalspac, const int* stGlobaltemp, const int* stGlobalspac, \
	const int* stLocaltemp, const int* stLocalspac, const int& dataLocaSize, \
	const int* branchJumpDistance, const int* branchBasicBlockSize, \
	const Branch& branch, const int& branchSize, const int* serialBlockSize, \
	const int& serialSize, const uint32_t armInstNum, const uint32_t thumbInstNum, \
	const uint32_t jazelleInstNum, const uint32_t aarch64InstNum) {

	ofstream traceResult(file_fea_path, ios::app);
	ofstream traceResultMica(file_mica_path, ios::app);

	//----------------InstMix------------------  // by shen 10/24
	traceResult << "-----------------InstMix------------------" << endl;
	traceResult << "intInstNum:	" << mix.intInstNum << endl;
	traceResult << "fpInstNum:	" << mix.fpInstNum << endl;
	traceResult << "SIMDInstNum:	" << mix.SIMDInstNum << endl;
	traceResult << "loadInstNum:	" << mix.loadInstNum << endl;
	traceResult << "storeInstNum:	" << mix.storeInstNum << endl;
	traceResult << "branchInstNum:	" << mix.branchInstNum << endl;
	traceResult << "serialInstNum:	" << mix.serialInstNum << endl;
	traceResult << "intAluInstNum:	" << mix.intAluNum << endl;
	traceResult << "intMultiDivNum:	" << mix.intMultiDivNum << endl;
	traceResult << "intMulNum:	" << mix.intMulNum << endl;
	traceResult << "fpNeonAluNum:	" << mix.fpNeonAluNum << endl;
	traceResult << "fpNeonDivNum:	" << mix.fpNeonDivNum << endl;
	traceResult << "fpNeonMulNum:	" << mix.fpNeonMulNum << endl;
	traceResult << "unknownNum:	" << mix.unKnownNum << endl;

	//--------------CriticalPath--------------------
	traceResult << "--------------CriticalPath--------------------" << endl;
	for (int i = 0; i < critSize; ++i)
		traceResult << "criticalPathLength[" << i << "]: " << critical[i] << endl;

	//--------------RegDependent--------------------
	traceResult << "--------------RegDependent--------------------" << endl;
	for (int i = 0; i < regDepSize; ++i)
		traceResult << "regDepLen[" << i << "]: " << regDepLen[i] << endl;

	traceResult << "intRegReadNum:	" << trace.intRegReadNum << endl;
	traceResult << "intRegWriteNum:	" << trace.intRegWriteNum << endl;
	traceResult << "fpRegReadNum:	" << trace.fpRegReadNum << endl;
	traceResult << "fpRegWriteNum:	" << trace.fpRegWriteNum << endl;
	traceResult << "ccRegWriteNum:	" << trace.ccRegWriteNum << endl;

	//--------------InstLocality--------------------
	traceResult << "--------------InstLocality--------------------" << endl;
	//instruction temporal locality
	for (int i = 0; i < instLocaSize; ++i)
		traceResult << "fetchReuseDist[" << i << "]: " << fetchReuseDist[i] << endl;
	//instruction spacial locality
	for (int i = 0; i < instLocaSize; ++i)
		traceResult << "fetchAddrDist[" << i << "]: " << fetchAddrDist[i] << endl;


	//--------------DataLocality--------------------
	traceResult << "--------------DataLocality--------------------" << endl;
	//load instruction global temporal locality
	for (int i = 0; i < dataLocaSize; ++i)
		traceResult << "ldGlobaltemp[" << i << "]: " << ldGlobaltemp[i] << endl;
	//load instruction local temporal locality
	for (int i = 0; i < dataLocaSize; ++i)
		traceResult << "ldLocaltemp[" << i << "]: " << ldLocaltemp[i] << endl;
	//store instruction global temporal locality
	for (int i = 0; i < dataLocaSize; ++i)
		traceResult << "stGlobaltemp[" << i << "]: " << stGlobaltemp[i] << endl;
	//store instruction global temporal locality
	for (int i = 0; i < dataLocaSize; ++i)
		traceResult << "stLocaltemp[" << i << "]: " << stLocaltemp[i] << endl;


	//load instruction global spacial locality
	for (int i = 0; i < dataLocaSize; ++i)
		traceResult << "ldGlobalspac[" << i << "]: " << ldGlobalspac[i] << endl;
	//load instruction local spacial locality
	for (int i = 0; i < dataLocaSize; ++i)
		traceResult << "ldLocalspac[" << i << "]: " << ldLocalspac[i] << endl;
	//store instruction global spacial locality
	for (int i = 0; i < dataLocaSize; ++i)
		traceResult << "stGlobalspac[" << i << "]: " << stGlobalspac[i] << endl;
	//store instruction global spacial locality
	for (int i = 0; i < dataLocaSize; ++i)
		traceResult << "stLocalspac[" << i << "]: " << stLocalspac[i] << endl;

	//---------------BranchBehavior-------------------
	traceResult << "---------------BranchBehavior-------------------" << endl;
	// basic block size between two branchs
	for (int i = 0; i < branchSize; ++i)
		traceResult << "branchBasicBlockSize[" << i << "]: " << branchBasicBlockSize[i] << endl;

	// branch behavior
	traceResult << "fwBranchs:		" << branch.fwBranchs << endl;
	traceResult << "bwBranchs:		" << branch.bwBranchs << endl;
	traceResult << "notTakenBranchs:		" << branch.notTakenBranchs << endl;
	traceResult << "takenChanged:		" << branch.takenChanged << endl;


	// jump distance after a branch instruction
	for (int i = 0; i < branchSize; ++i)
		traceResult << "branchJumpDistance[" << i << "]: " << branchJumpDistance[i] << endl;

	//---------------SerialInstDist-------------------
	traceResult << "---------------SerialInstDist-------------------" << endl;
	// basic block size between two branchs
	for (int i = 0; i < serialSize; ++i)
		traceResult << "serialBlockSize[" << i << "]: " << serialBlockSize[i] << endl;

	//---------------Instrution Set-------------------
	traceResult << "---------------InstSet-------------------" << endl;
	traceResult << "armInstNum:	" << armInstNum << endl;
	traceResult << "thumbInstNum:	" << thumbInstNum << endl;
	traceResult << "jazelleInstNum:	" << jazelleInstNum << endl;
	traceResult << "aarch64InstNum:	" << aarch64InstNum << endl;
	// traceResult end


	// traceResultMica
	//----------------InstMix------------------
	traceResultMica << mix.intInstNum << ", "; // by shen 10/24
	traceResultMica << mix.fpInstNum << ", ";
	traceResultMica << mix.SIMDInstNum << ", ";
	traceResultMica << mix.loadInstNum << ", ";
	traceResultMica << mix.storeInstNum << ", ";
	traceResultMica << mix.branchInstNum << ", ";
	traceResultMica << mix.serialInstNum << ", ";
	traceResultMica << mix.intAluNum << ", ";
	traceResultMica << mix.intMultiDivNum << ", ";
	traceResultMica << mix.intMulNum << ", ";
	traceResultMica << mix.fpNeonAluNum << ", ";
	traceResultMica << mix.fpNeonDivNum << ", ";
	traceResultMica << mix.fpNeonMulNum << ", ";
	traceResultMica << mix.unKnownNum << ", ";

	//--------------CriticalPath--------------------
	for (int i = 0; i < critSize; ++i)
		traceResultMica << critical[i] << ", ";

	//--------------RegDependent--------------------
	for (int i = 0; i < regDepSize; ++i)
		traceResultMica << regDepLen[i] << ", ";

	traceResultMica << trace.intRegReadNum << ", ";
	traceResultMica << trace.intRegWriteNum << ", ";
	traceResultMica << trace.fpRegReadNum << ", ";
	traceResultMica << trace.fpRegWriteNum << ", ";
	traceResultMica << trace.ccRegWriteNum << ", ";

	//--------------InstLocality--------------------
	//instruction temporal locality
	for (int i = 0; i < instLocaSize; ++i)
		traceResultMica << fetchReuseDist[i] << ", ";
	//instruction spacial locality
	for (int i = 0; i < instLocaSize; ++i)
		traceResultMica << fetchAddrDist[i] << ", ";


	//--------------DataLocality--------------------
	//load instruction global temporal locality
	for (int i = 0; i < dataLocaSize; ++i)
		traceResultMica << ldGlobaltemp[i] << ", ";
	//load instruction local temporal locality
	for (int i = 0; i < dataLocaSize; ++i)
		traceResultMica << ldLocaltemp[i] << ", ";
	//store instruction global temporal locality
	for (int i = 0; i < dataLocaSize; ++i)
		traceResultMica << stGlobaltemp[i] << ", ";
	//store instruction global temporal locality
	for (int i = 0; i < dataLocaSize; ++i)
		traceResultMica << stLocaltemp[i] << ", ";

	//load instruction global spacial locality
	for (int i = 0; i < dataLocaSize; ++i)
		traceResultMica << ldGlobalspac[i] << ", ";
	//load instruction local spacial locality
	for (int i = 0; i < dataLocaSize; ++i)
		traceResultMica << ldLocalspac[i] << ", ";
	//store instruction global spacial locality
	for (int i = 0; i < dataLocaSize; ++i)
		traceResultMica << stGlobalspac[i] << ", ";
	//store instruction global spacial locality
	for (int i = 0; i < dataLocaSize; ++i)
		traceResultMica << stLocalspac[i] << ", ";

	//---------------BranchBehavior-------------------
	// basic block size between two branchs
	for (int i = 0; i < branchSize; ++i)
		traceResultMica << branchBasicBlockSize[i] << ", ";

	// branch behavior
	traceResultMica << branch.fwBranchs << ", ";
	traceResultMica << branch.bwBranchs << ", ";
	traceResultMica << branch.notTakenBranchs << ", ";
	traceResultMica << branch.takenChanged << ", ";

	// jump distance after a branch instruction
	for (int i = 0; i < branchSize; ++i)
		traceResultMica << branchJumpDistance[i] << ", ";

	//---------------SerialInstDist-------------------
	// basic block size between two branchs
	for (int i = 0; i < serialSize - 1; ++i)
		traceResultMica << serialBlockSize[i] << ", ";
	traceResultMica << serialBlockSize[serialSize - 1];

	//---------------Instrution Set-------------------
	traceResultMica << ", " << armInstNum;
	traceResultMica << ", " << thumbInstNum;
	traceResultMica << ", " << jazelleInstNum;
	traceResultMica << ", " << aarch64InstNum<<endl;

	traceResultMica.close();
	traceResult.close();
}