#include <string.h>

void *memcpy(void *__restrict dest, const void *__restrict src, size_t count)
{
	unsigned char *d = (unsigned char *)dest;
	const unsigned char *s = (const unsigned char *)src;

	while (count--) {
		*d++ = *s++;
	}

	return dest;
}
