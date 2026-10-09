#include <stdio.h>
#include <stdlib.h>

__attribute__((__noreturn__)) void abort(void)
{
#if defined(__is_libk)
	//printf("abort()\n");
	asm volatile("hlt");
#else
	// TODO: implement userspace SIGABRT
#endif

	// NOTE: to stop this function from returning void?
	while (1) {
	}
	__builtin_unreachable();
}
