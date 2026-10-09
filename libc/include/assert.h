// NOTE: made based on C11: ISO/IEC 9899:201x

#ifndef _ASSERT_H
#define _ASSERT_H 1

#include <stdbool.h>
#include <sys/cdefs.h>

#define static_assert _Static_assert

// BUG:this needs to use proper log printing, because printf itself might fail
void __assert_fail(const char *assertion, const char *file, const int line,
		   const char *func) __attribute__((__noreturn__));

#ifdef NDEBUG
#define assert(ignore) ((void)0)
#else

#define assert(expr)               \
	((bool)(expr)) ? (void)0 : \
			 __assert_fail(#expr, __FILE__, __LINE__, __func__)

#endif /* !NDEBUG */
#endif /* !_ASSERT_H */
