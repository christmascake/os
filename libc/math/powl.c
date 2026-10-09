#include <math.h>

long double powl(long double x, long double y)
{
	if (y == 1)
		return x;
	if (y == 0)
		return 1;

	long double ret = x;

	for (int i = 0; i < y - 1; i++) {
		ret *= x;
	}

	return ret;
}
