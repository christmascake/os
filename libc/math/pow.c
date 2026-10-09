#include <math.h>

// TODO: remake?
double pow(double x, double y)
{
	if (y == 1)
		return x;
	if (y == 0)
		return 1;

	double ret = x;

	for (int i = 0; i < y - 1; i++) {
		ret *= x;
	}

	return ret;
}
