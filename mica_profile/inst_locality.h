//
// This is to calculate instruction locality
// inst_paddr: the latest instruction's physical address
//
// Created by Fu Wenjie
//

#ifndef INSTLOCALITY_H
#define INSTLOCALITY_H

#define INST_LOCA_D_SIZE	10  // by fuwenjie 10/14

class InstLocality {
	deque<uint64_t> fetchAddrList;
	//int *fetchReuseDist;
	//int *fetchAddrDist;
	int fetchReuseDist[INST_LOCA_D_SIZE];
	int fetchAddrDist[INST_LOCA_D_SIZE];
	int distSize; /* distribution array size */

public:
	/* set distribution array size , default is 11 */
	InstLocality(const int & size) : distSize(size) {  ////
		/*fetchReuseDist = new int[distSize];
		fetchAddrDist = new int[distSize];*/

		for (int i = 0; i < distSize; ++i) {
			fetchReuseDist[i] = 0;
			fetchAddrDist[i] = 0;
		}
	}

	InstLocality() : InstLocality(INST_LOCA_D_SIZE) {};

	~InstLocality() {
		//delete[] fetchReuseDist;
		//delete[] fetchAddrDist;
	}

	void instLocality(const InstTrace & instTrace);

	const int * getFetchReuseDist() const { return fetchReuseDist; }

	const int * getFetchAddrDist() const { return fetchAddrDist; }

	const int & getSize() const { return distSize; }
};

#endif
