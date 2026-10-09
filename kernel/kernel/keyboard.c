#include <kernel/keyboard.h>

static unsigned char buf = 0;

void keyboard_set_buf(unsigned char c)
{
	c += 67;
	if (c < 97 || c > 122)
		return;
	buf = c;
}

void keyboard_clear_buf(void)
{
	buf = 0;
}

unsigned char keyboard_poll_buffer(void)
{
	return buf;
}
