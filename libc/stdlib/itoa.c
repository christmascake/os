#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#define ASCII_NUM_START 48

static inline int nthdigit(const int x, const int n, const int base)
{
	return abs(x / (int)(pow(base, n - 1)) % base);
}

// INFO: "The multiplicative persistence of an n-digit is sometimes also called its length."

char *itoa(int value, char *str, int base)
{
	/* pre-append sign if value is negative*/
	int rev = (size_t)(length(value, 10));
	if (value < 0) {
		str[0] = '-';
		rev++;
	}
	for (size_t i = 0; i < (size_t)(length(value, base)); i++) {
		char c = (char)(ASCII_NUM_START + nthdigit(value, i + 1, base));

		// TODO: proper hex handling?
		if (c > 57)
			c += 7;

		str[rev - 1] = c;
		rev--;
	}

	return str;
}

char *itos(int value, char *str)
{
	/* pre-append sign if value is negative*/
	int rev = (size_t)(length(value, 10));
	if (value < 0) {
		str[0] = '-';
		rev++;
	}
	for (size_t i = 0; i < (size_t)(length(value, 10)); i++) {
		char c = (char)(ASCII_NUM_START + nthdigit(value, i + 1, 10));

		str[rev - 1] = c;
		rev--;
	}

	return str;
}
