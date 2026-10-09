#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <i386/include/serial.h>
#include <i386/include/io.h>

#define COM1_PORT 0x3F8

#define COM1_MASK_TEMT 0x02

static inline bool is_transmit_empty(void)
{
	return io_inb(COM1_PORT + 5) & COM1_MASK_TEMT;
}

int init_serial(void)
{
	io_outb(COM1_PORT + 1, 0x00); // disable all interrupts
	io_outb(COM1_PORT + 3, 0x80); // DLAB 1
	io_outb(COM1_PORT, 0x03); // set divisor to 3 (low byte)
	io_outb(COM1_PORT + 1, 0x03); // (high byte)
	io_outb(COM1_PORT + 3, 0x03); // 8 char bits, no parity, 1 stop bit
	io_outb(COM1_PORT + 2, 0xC7); // set FIFO, clear them, 14 byte threshold
	io_outb(COM1_PORT + 4, 0x0B); // IRQs enabled, RTS/DSR set

	io_outb(COM1_PORT + 4, 0x1E); // Set in loopback mode, test serial chip

	/* Test serial chip (send byte 0xAE and check if serial returns same byte) */
	io_outb(COM1_PORT, 0xAE);

	/* failure */
	if (io_inb(COM1_PORT) != 0xAE)
		return 1;

	/* success */
	io_outb(COM1_PORT + 4, 0x0F);
	return 0;
}

void serial_putchar(const char c)
{
	while (is_transmit_empty());
	io_outb(COM1_PORT, c);
}

void serial_fctputchar(char c, void *arg)
{
	(void)arg;
	serial_putchar(c);
}

void serial_write(const char *str, const size_t n)
{
	for (size_t i = 0; i < n; i++) {
		serial_putchar(str[i]);
	}
}

void serial_print(const char *str)
{
	for (size_t i = 0; i < strlen(str); i++) {
		serial_putchar(str[i]);
	}
}
