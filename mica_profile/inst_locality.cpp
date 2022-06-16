//
// This is to calculate instruction locality
// inst_paddr: the latest instruction's physical address
//
// Created by Fu Wenjie
//

#include "global_def.h"
#include "inst_locality.h"

#define MAX_LIST_SIZE	1023  // by fuwenjie  10/14

void InstLocality::instLocality(const InstTrace & instTrace) {
	//int realFetchAddr = instTrace.inst_paddr & 0x00000000FFFFFFFF;
	uint64_t realFetchAddr = instTrace.inst_paddr;					 //[modified by pz] 对64位指令，不应该清空高32位

	deque<uint64_t>::iterator it;
	/* for first time -- the following process need a not empty list*/
	if (fetchAddrList.empty()) {
		fetchAddrList.push_front(realFetchAddr);	
		return;
	}
	/* calculate instruction's temporary locality(amount of insts) */
	int instNum = 0;
	int reuseTmp = 0;
	for (it = fetchAddrList.begin(); it != fetchAddrList.end(); ++it){
		if (*it == realFetchAddr) break;
		else ++instNum;
	}

	/* if not found the same addr */
	if (instNum == fetchAddrList.size()) {
		instNum = 0;
		--fetchReuseDist[instNum];
	}

	while (instNum){   // [LHX] 取2的对数
		instNum >>= 1;
		++reuseTmp;
	}
	if (reuseTmp > distSize - 1) 	// [LHX] 大于distSize - 1都落于distSize - 1
		reuseTmp = distSize - 1;
	++fetchReuseDist[reuseTmp];

	/* calculate instruction's spacial locality */
	int spac = 0;
	uint64_t addrNum = minus64(*(--(fetchAddrList.end())), realFetchAddr); //????????
	while (addrNum){
		addrNum >>= 1;
		++spac;
	}

	if (spac > distSize - 1) 
		spac = distSize - 1;
	++fetchAddrDist[spac];

	fetchAddrList.push_front(realFetchAddr);

	/* limit the instruction quene size up to 512 */
	if (fetchAddrList.size() == MAX_LIST_SIZE)
		fetchAddrList.pop_back();
}
