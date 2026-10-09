#include <string.h>

int memcmp(const void *lhs, const void *rhs, size_t count)
{
	const unsigned char *l = (const unsigned char *)lhs;
	const unsigned char *r = (const unsigned char *)rhs;

	for (size_t i = 0; i < count; i++) {
		if (l[i] < r[i])
			return -1;
		else if (l[i] > r[i])
			return 1;
	}

	return 0;
}
