//
// This is to calculate the block size of serial instruction
// instType: to identify whether it is a serial instruction
//
// Created by Fu Wenjie
//

#include "global_def.h"
#include "serial_inst_dist.h"

void SerialInstDist::serialInstDist(const bool &isSerial)
{
	/* if first meet serialInst, ignore */
	static bool flag = false;
	if (!isSerial && !flag)
		return;
	else if (isSerial && !flag) {
		flag = true;
		return;
	}
	static uint64_t serialNum = 0;
	/* if it's not a serial instruction, only increase count number */
	if (!isSerial)
		serialNum++;
	else {
		int size = 0;

		while (serialNum){
			size++;
			serialNum >>= 1;
		}
		if (size > distSize - 1)
			size = distSize - 1;

		++serialBlockSize[size];

		serialNum = 0;
	}
}
