#include <math.h>

double floor(double x)
{
	//if (!isfinite(x) || x == 0.0)
	//	return x;

	//const double TWO_POW_53 = 9007199254740992.0; /* 2^53 */
	//if (fabs(x) >= TWO_POW_53)
	//	return x;

	long long t = (long long)x;

	if (x < 0.0 && (double)t != x)
		return (double)(t - 1);

	return (double)t;
}
