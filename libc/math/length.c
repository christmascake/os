#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int length(const int n, const int base)
{
	if (n == 0)
		return 1;

	int t	= abs(n);
	int len = 0;

	while (t > 0) {
		t /= base;
		len++;
	}

	return len;
}
