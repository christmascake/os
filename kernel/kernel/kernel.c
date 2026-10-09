#ifdef __arch_i386

#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <kernel/tty.h>
#include <kernel/mb2header.h>
#include <kernel/psf2.h>
#include <kernel/font.h>
#include <kernel/log.h>
#include <kernel/framebuffer.h>
#include <kernel/keyboard.h>

#include <i386/include/gdt.h>
#include <i386/include/idt.h>
#include <i386/include/pic.h>
#include <i386/include/io.h>
#include <i386/include/serial.h>

void kernel_main(unsigned long magic, unsigned long addr)
{
	init_gdt();
	init_idt();
	init_serial();
	init_mb2header(magic, addr);
	init_font(_binary_font_cp850_8x16_psfu_start);
	init_tty(addr);

	while (1) {
		const auto c = keyboard_poll_buffer();

		if (c) {
			printf("%c", c);
		}

		keyboard_clear_buf();
		asm volatile("hlt");
	}
}

#endif
