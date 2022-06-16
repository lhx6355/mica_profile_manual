//
// This is to calculate branch behaviors
// inst: the latest Inst struct
// nextInst: the next Inst struct
// instType: type of the opcode
// branch: stores branch behaviors
//
// Created by Fu Wenjie
//

#ifndef BRANCHBEHAVIOR_H
#define BRANCHBEHAVIOR_H

#define B_DIST_SIZE		15  // by fuwenjie 10/14

class BranchBehavior {
	uint32_t distSize; /* distribution array size */
	int branchJumpDistance[B_DIST_SIZE]; // by shen 10/17
	int branchBasicBlockSize[B_DIST_SIZE];
	int instLength;			//[modified by pz]
	Branch branch;

public:
	/* constructor for setting the size of statistic info,
	   default is 11 */
	BranchBehavior(const int & size) : distSize( size ) {
		//branchJumpDistance = new int[size];
		//branchBasicBlockSize = new int[size];
		
		for (uint32_t i = 0; i < distSize; ++i) {
			branchJumpDistance[i] = 0;
			branchBasicBlockSize[i] = 0;
		}
	}
	
	BranchBehavior() : BranchBehavior(B_DIST_SIZE) {};

	~BranchBehavior() {
		//delete[] branchBasicBlockSize;
		//delete[] branchJumpDistance;
	}
	
	void calBranchBehavior(const InstTrace &inst, const InstTrace &nextInst, const bool &isBranch);

	const int * getBraJumDist() const { return branchJumpDistance; }

	const int * getBraBasBlock() const { return branchBasicBlockSize; }

	const Branch & getBranch() const { return branch; }

	const uint32_t & getSize() const { return distSize; }
};

#endif
