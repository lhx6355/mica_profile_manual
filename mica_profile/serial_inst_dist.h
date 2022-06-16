//
// This is to calculate the block size of serial instruction
// instType: to identify whether it is a serial instruction
//
// Created by Fu Wenjie
//

#ifndef SERIALINSTDIST_H
#define SERIALINSTDIST_H
#include "global_def.h"

#define D_SIZE	15

class SerialInstDist {
	//int *serialBlockSize;
	int serialBlockSize[D_SIZE];
	int distSize; /* distribution array size */

public:
	/* set distribution array size , default is 20 */
	SerialInstDist(const int & size) : distSize(size) {
		//serialBlockSize = new int[distSize];
		for (int i = 0; i < distSize; ++i) 
			serialBlockSize[i] = 0;
	}

	SerialInstDist() : SerialInstDist(D_SIZE) {};

	~SerialInstDist() {
		//delete[] serialBlockSize;
	}

	void serialInstDist(const bool &isSerial);

	const int * getSerialBlock() const { return serialBlockSize; }

	const int & getSize() const { return distSize; }
};

#endif
