//
// This is to calculate the instruction's mix ratio
// instTrace: the items that we are tracing
// instA64Counters: the number of instruction sets that are used
//
// Created by Fu Wenjie
//

#ifndef INST_MIX_RATIO
#define INST_MIX_RATIO

#include "global_def.h"

class InstMixRatio {
	InstMix instMix;

public :
	InstMixRatio() {}
	void instMixRatio(const int *instA64Counters, const int *instA32Counters);

	~InstMixRatio() {}

	InstMix & getInstMix() { return instMix; }

};

#endif
