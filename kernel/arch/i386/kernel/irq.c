#include <stdint.h>
#include <stdio.h>

#include <kernel/keyboard.h>

#include <i386/include/io.h>
#include <i386/include/pic.h>

#define IRQ0_SYSTEM_TIMER	    0
#define IRQ1_KEYBOARD		    1
#define IRQ2_CASCADE		    2
#define IRQ3_SERIAL2_PORT	    3
#define IRQ4_SERIAL1_PORT	    4
#define IRQ5_SOUND_CARD		    5
#define IRQ6_FLOPPY_DISK_CONTROLLER 6
#define IRQ7_PARALLEL1_PORT	    7
#define IRQ8_REAL_TIME_CLOCK	    8
#define IRQ9_OPEN_INTERRUPT1	    9
#define IRQ10_OPEN_INTERRUPT2	    10
#define IRQ11_OPEN_INTERRUPT3	    11
#define IRQ12_PS2_MOUSE		    12
#define IRQ13_FPU		    13
#define IRQ14_IDE_PRIMARY	    14
#define IRQ15_IDE_SECONDARY	    15

struct regs {
	uint32_t edi, esi, ebp, eso, ebx, edx, ecx, eax;
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
} __attribute__((__packed__));

static inline void handle_keyboard(const struct regs *reg)
{
	/* WARNING:we want to do as little as possible before PIC_send_EOI */
	keyboard_set_buf(io_inb(0x60));
}

void irq_handle_common(struct regs *reg, uint32_t irq)
{
	/* TODO: handle spurious irqs */

	switch (irq) {
	case IRQ1_KEYBOARD: handle_keyboard(reg); break;
	case IRQ4_SERIAL1_PORT: puts("serial1port"); break;
	case IRQ7_PARALLEL1_PORT:
	case IRQ15_IDE_SECONDARY: puts("spurious"); break;
	default: break;
	}

	PIC_send_EOI(irq);
	/* handle the keyboard buffer here! */
}
