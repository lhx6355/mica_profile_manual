//
// This is to calculate data locality
// call function DataLocality(...) in main()
// inst: the latest Inst struct
// instType: type of the opcode 
//
// Created by Fu Wenjie
//


#include "data_locality.h"
#define MAX_QUE_SIZE	4095 // by shen 10/15

inline void DataLocality::ldStDataLocality(const InstTrace &instTrace, deque<InstTrace> &ldStQue, uint64_t &preLdSt_paddr, uint64_t &preLdSt_pc, \
	int *ldStGlobaltemp, int *ldStGlobalspac, int *ldStLocaltemp, int *ldStLocalspac, const bool &isLoad) {
	//[pz]应分别为LOAD,STORE使用两个First标志变量，而不是共用一个ldstFirst
	static bool ldStFirst = false;

	if (!ldStFirst) {
		ldStFirst = true;
		ldStQue.push_front(instTrace);
		return;
	}

	int reUse = 0;
	int count = 0;
	int reUseGlobal = 0;
	int countGlobal = 0;
	/* if this instTrace is a load instruction,change the paddr */
	uint64_t quePaddr; // by shenshan 10/13
	uint64_t ldSt_paddr;
	if (isLoad)
		ldSt_paddr = instTrace.load_paddr;
	else			
		ldSt_paddr = instTrace.store_paddr;

	deque<InstTrace>::iterator it;
	/* tracing load or store Local/Global reUseDist */
	for (it = ldStQue.begin(); it != ldStQue.end(); it++){
		if (isLoad) // by shen 10/13
			quePaddr = it->load_paddr;
		else		
			quePaddr = it->store_paddr;
		/* to find the instruction that has same load physical addreess in the load quene */
		if (quePaddr == ldSt_paddr) //shen 10/13
			break;
		else
			reUseGlobal++;
	}
	/* if all items are different, no item's load_paddr is equal GlobalTemp
	should minus by itself because it will increase later by accident */
	if (reUseGlobal == ldStQue.size()) {
		reUseGlobal = 0;
		--ldStGlobaltemp[reUseGlobal];
	}
	while (reUseGlobal){
		reUseGlobal >>= 1;
		++countGlobal;
	}
	if (countGlobal > distSize - 1) 
		countGlobal = distSize - 1;
	/* statistic reuse distance (num of insts) distribution */
	++ldStGlobaltemp[countGlobal];

	for (it = ldStQue.begin(); it != ldStQue.end(); it++){
		if (isLoad)
			quePaddr = it->load_paddr;
		else
			quePaddr = it->store_paddr;

		if ((it->pc == instTrace.pc) && (quePaddr == ldSt_paddr)) // by shen 10/12
			break;
		else
			++reUse;	
	}
	if (reUse == ldStQue.size()) {
		reUse = 0;					
		--ldStLocaltemp[reUse];
	}
	while (reUse){
		reUse >>= 1;
		++count;
	}
	
	if (count > distSize - 1)
		count = distSize - 1;
	++ldStLocaltemp[count];

	/* tracing ldst Local/Global access memory address distance */
	uint64_t tmpDistAddr = minus64(preLdSt_paddr, ldSt_paddr); // by shen 10/13

	countGlobal = 0;
	/* the spac of address is power of 8 */
	while (tmpDistAddr){
		tmpDistAddr >>= 3; //?????????
		++countGlobal;
	}

	// by shen 11/13
	/* physical memorary is limited to 2G */
	if (countGlobal < distSize) {
		++ldStGlobalspac[countGlobal];
		if (preLdSt_pc == instTrace.pc)
			++ldStLocalspac[countGlobal];
	}

	/* limit load quene and store quene size up to 4095 */
	if (ldStQue.size() == MAX_QUE_SIZE)
		ldStQue.pop_back();

	ldStQue.push_front(instTrace);
}

void DataLocality::dataLocality(const InstTrace &instTrace, const InstType &instType) {
	static uint64_t preLd_paddr;
	static uint64_t preLd_pc;
	static uint64_t preSt_paddr;
	static uint64_t preSt_pc;
#ifdef DEBUG
	cout << hex << instTrace.pc << " : " << instTrace.load_paddr << " , " << instTrace.store_paddr << endl; // shen 10/12
#endif
	if (instType.isLoad){
		ldStDataLocality(instTrace, ldQueue, preLd_paddr, preLd_pc, \
			ldGlobaltemp, ldGlobalspac, ldLocaltemp, ldLocalspac, instType.isLoad);
		/* update preLdSt */
		preLd_paddr = instTrace.load_paddr;
		preLd_pc = instTrace.pc;
	}
	else if (instType.isStore) {
		ldStDataLocality(instTrace, stQueue, preSt_paddr, preSt_pc, \
			stGlobaltemp, stGlobalspac, stLocaltemp, stLocalspac, instType.isLoad);
		/* update preSt */
		preSt_paddr = instTrace.store_paddr;// by shenshan 10/12
		preSt_pc = instTrace.pc;
	}
}