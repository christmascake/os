#ifndef ORUOS_KERNEL_ARCH_I386_INCLUDE_SERIAL_H_
#define ORUOS_KERNEL_ARCH_I386_INCLUDE_SERIAL_H_

#include <stddef.h>

int init_serial(void);

void serial_putchar(const char c);
void serial_write(const char *str, const size_t n);
void serial_print(const char *str);

void serial_fctputchar(char c, void *arg);

#endif
