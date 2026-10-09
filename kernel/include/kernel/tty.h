#ifndef ORUOS_KERNEL_INCLUDE_KERNEL_TTY_H_
#define ORUOS_KERNEL_INCLUDE_KERNEL_TTY_H_

#include <stddef.h>

void init_tty(const unsigned int addr);
void tty_putchar(const char c);
void tty_write(const char *str, const size_t n);
void tty_write_string(const char *str);

#endif
