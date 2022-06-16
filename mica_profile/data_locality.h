//
// This is to calculate data locality
// call function DataLocality(...) in main()
// inst: the latest Inst struct
// instType: type of the opcode 
//
// Created by Fu Wenjie
//

#ifndef DATALOCALITY_H
#define DATALOCALITY_H

#include "global_def.h"

#define D_DIST_SIZE	12  // by shen 10/19

class DataLocality {
	int distSize;
	deque<InstTrace> ldQueue;
	deque<InstTrace> stQueue;
	/*int *ldGlobaltemp;
	int *ldGlobalspac;
	int *ldLocaltemp;
	int *ldLocalspac;
	int *stGlobaltemp;
	int *stGlobalspac;
	int *stLocaltemp;
	int *stLocalspac;*/
	int ldGlobaltemp[D_DIST_SIZE];
	int ldGlobalspac[D_DIST_SIZE];
	int ldLocaltemp[D_DIST_SIZE];
	int ldLocalspac[D_DIST_SIZE];
	int stGlobaltemp[D_DIST_SIZE];
	int stGlobalspac[D_DIST_SIZE];
	int stLocaltemp[D_DIST_SIZE];
	int stLocalspac[D_DIST_SIZE];

public:
	/* set distribution array size ,default is 11 */
	DataLocality(const int & size) : distSize( size ) {
		//ldGlobaltemp = new int[distSize]; /* global reuse distance(amount of insts) for load inst on the time */
		//ldGlobalspac = new int[distSize]; /* global reuse distance(address difference of insts) for load inst on the space */
		//ldLocaltemp  = new int[distSize]; /* local, pc of two loads are same */
		//ldLocalspac  = new int[distSize];
		//stGlobaltemp = new int[distSize];
		//stGlobalspac = new int[distSize];
		//stLocaltemp  = new int[distSize];
		//stLocalspac  = new int[distSize];

		for (int i = 0; i < distSize; ++i) {
			ldGlobaltemp[i] = 0;
			ldGlobalspac[i] = 0;
			ldLocaltemp [i] = 0;
			ldLocalspac [i] = 0;
			stGlobaltemp[i] = 0;
			stGlobalspac[i] = 0;
			stLocaltemp [i] = 0;
			stLocalspac [i] = 0;
		}
	}

	DataLocality() : DataLocality( D_DIST_SIZE ) {}

	~DataLocality() {
		//delete[] ldGlobaltemp;
		//delete[] ldGlobalspac;
		//delete[] ldLocaltemp;
		//delete[] ldLocalspac;
		//delete[] stGlobaltemp;
		//delete[] stGlobalspac;
		//delete[] stLocaltemp;
		//delete[] stLocalspac;
	}

	inline void ldStDataLocality(const InstTrace &instTrace, deque<InstTrace> &ldStQue, uint64_t &preLdSt_paddr, uint64_t &preLdSt_pc, \
		int *ldStGlobaltemp, int *ldStGlobalspac, int *ldStLocaltemp, int *ldStLocalspac, const bool &isLoad);

	void dataLocality(const InstTrace &instTrace, const InstType &instType);

	const int * getLdGlobalTemp() const { return ldGlobaltemp; }

	const int * getLdGlobalSpac() const { return ldGlobalspac; }

	const int * getLdLocalTemp() const { return ldLocaltemp; }

	const int * getLdLocalSpac() const { return ldLocalspac; }

	const int * getStGlobalTemp() const { return stGlobaltemp; }

	const int * getStGlobalSpac() const { return stGlobalspac; }

	const int * getStLocalTemp() const { return stLocaltemp; }

	const int * getStLocalSpac() const { return stLocalspac; }

	const int & getSize() const { return distSize; }
};

#endif
