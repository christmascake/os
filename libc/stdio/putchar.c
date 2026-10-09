#include <stdio.h>

#if defined(__is_libk)
#include <kernel/tty.h>
#endif

#if defined(__is_libk)

// TODO:correct error handling (maybe hosted only?)
int putchar(int ch)
{
	tty_putchar(ch);
	return ch;
}

#elif defined(__is_hosted_i386)
/* hosted i386 impl */
#endif
