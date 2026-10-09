#include <math.h>

float powf(float x, float y)
{
	if (y == 1)
		return x;
	if (y == 0)
		return 1;

	float ret = x;

	for (int i = 0; i < y - 1; i++) {
		ret *= x;
	}

	return ret;
}
