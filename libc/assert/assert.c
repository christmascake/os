#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

void __assert_fail(const char *assertion, const char *file, const int line,
		   const char *func)
{
	printf("assertion failed:(%s), file:%s, line:%d, func:%s\n", assertion,
	       file, line, func);

	abort();
}
