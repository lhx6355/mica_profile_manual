#ifndef STATISTIC_TRACE
#define STATISTIC_TRACE

#include "global_def.h"
#include "decode_aarch64.h"
#include "decode_aarch32.h"
#include "inst_mix_ratio.h"
#include "calc_critical_path.h"
#include "reg_depend_length.h"
#include "inst_locality.h"
#include "data_locality.h"
#include "branch_behavior.h"
#include "serial_inst_dist.h"


/*void printInfo(const char *path,const char *name, const InstMix & mix, const int *critical, const int &robSize, \
	const int *regDepLen, const Trace &trace, const int &regDepSize, \
	const int *fetchReuseDist, const int *fetchAddrDist, const int &instLocaSize, \
	const int *ldGlobaltemp, const int *ldGlobalspac, const int *ldLocaltemp, \
	const int *ldLocalspac, const int *stGlobaltemp, const int *stGlobalspac, \
	const int *stLocaltemp, const int *stLocalspac, const int & dataLocaSize, \
	const int *branchJumpDistance, const int *branchBasicBlockSize, \
	const Branch &branch, const int &branchSize, const int *serialBlockSize, \
	const int &serialSize, const uint32_t armInstNum, const uint32_t thumbInstNum, \
	const uint32_t jazelleInstNum, const uint32_t aarch64InstNum);*/


void printInfo(const char* file_fea_path, const char* file_mica_path, \
	const InstMix & mix, const int *critical, const int &robSize, \
	const int *regDepLen, const Trace &trace, const int &regDepSize, \
	const int *fetchReuseDist, const int *fetchAddrDist, const int &instLocaSize, \
	const int *ldGlobaltemp, const int *ldGlobalspac, const int *ldLocaltemp, \
	const int *ldLocalspac, const int *stGlobaltemp, const int *stGlobalspac, \
	const int *stLocaltemp, const int *stLocalspac, const int & dataLocaSize, \
	const int *branchJumpDistance, const int *branchBasicBlockSize, \
	const Branch &branch, const int &branchSize, const int *serialBlockSize, \
	const int &serialSize, const uint32_t armInstNum, const uint32_t thumbInstNum, \
	const uint32_t jazelleInstNum, const uint32_t aarch64InstNum);

void statisticTrace(InstTrace * instTrace, const int &size,const char *file_fea_path,const char *file_mica_path);

void initLatency(int *latencyA32Ptr, int *latencyA64Ptr);

#endif