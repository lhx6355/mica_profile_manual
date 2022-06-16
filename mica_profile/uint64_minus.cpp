#include "global_def.h"

uint64_t minus64(const uint64_t &a, const uint64_t &b) {
	if (a > b)
		return a - b;
	else
		return b - a;
}