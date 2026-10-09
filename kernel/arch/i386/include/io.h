#ifndef ORUOS_KERNEL_ARCH_I386_INCLUDE_IO_H_
#define ORUOS_KERNEL_ARCH_I386_INCLUDE_IO_H_

#include <stdint.h>

inline void io_outb(const uint16_t port, const uint8_t val)
{
	__asm__ volatile("outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

inline uint8_t io_inb(const uint16_t port)
{
	uint8_t ret = 0;

	__asm__ volatile("inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory");

	return ret;
}

inline void io_wait(void)
{
	io_outb(0x80, 0);
}

#endif
