#include <math.h>

int concat(const int p, const int q, const int base)
{
	return p * pow(base, length(q, base)) + q;
}
