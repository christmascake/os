#include <string.h>

void *memset(void *dest, int ch, size_t count)
{
	unsigned char *buf = (unsigned char *)dest;

	while (count--)
		*buf++ = (unsigned char)ch;

	return dest;
}
