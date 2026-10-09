#include <stdio.h>
#include <stdint.h>

#include <i386/include/idt.h>
#include <i386/include/pic.h>

#define IDT_TOTAL_ENTRIES		256
#define IDT_TOTAL_CPU_EXCEPTION_ENTRIES 32
#define IDT_TOTAL_IRQ_ENTRIES		16

#define PIC1_OFFSET 0x20
#define PIC2_OFFSET 0x28

struct idt_entry {
	uint16_t isr_low; // offset 0..15
	uint16_t selector; // segment code selector in gdt
	uint8_t	 zero; // reserved
	uint8_t	 attributes; // P(1) -> DPL(2) -> (0)1 -> gate type(4)
	uint16_t isr_high; // offset 16..31
} __attribute__((__packed__));

static struct {
	uint16_t limit;
	uint32_t base;
} __attribute__((__packed__)) idtr = { .limit = 0, .base = 0 };

__attribute__((__aligned__(0x10))) static struct idt_entry idt_table[256];

/* this is cringe */
static bool vectors[IDT_TOTAL_ENTRIES];

extern void *isr_stub_table[];
extern void *irq_stub_table[];

static void set_descriptor(const uint8_t vector, void *isr, const uint8_t flags)
{
	struct idt_entry *desc	   = &idt_table[vector];
	const uint32_t	  isr_addr = (uint32_t)isr;
	desc->isr_low		   = (uint32_t)isr_addr & 0xFFFF;
	desc->selector		   = 0x8;
	desc->zero		   = 0;
	desc->attributes	   = flags;
	desc->isr_high		   = (uint16_t)((isr_addr >> 16) & 0xFFFF);
}

void init_idt(void)
{
	idtr.base  = (uint32_t)&idt_table[0];
	idtr.limit = (uint16_t)(sizeof(idt_table) - 1);

	/* clear everything */
	for (int i = 0; i < IDT_TOTAL_ENTRIES; i++) {
		((uint64_t *)idt_table)[i] = 0;
		vectors[i]		   = false;
	}

	/* setup cpu exception routines */
	for (uint8_t v = 0; v < IDT_TOTAL_CPU_EXCEPTION_ENTRIES; v++) {
		set_descriptor(v, isr_stub_table[v], 0x8E);
		vectors[v] = true;
	}

	/* setup irq routines */
	for (uint8_t v = 0; v < IDT_TOTAL_IRQ_ENTRIES; v++) {
		set_descriptor(PIC1_OFFSET + v, irq_stub_table[v], 0x8E);
		vectors[PIC1_OFFSET + v] = true;
	}

	/* init the PIC */
	PIC_remap(PIC1_OFFSET, PIC2_OFFSET);

	/* load the idt */
	__asm__ volatile("lidt %0" : : "m"(idtr));

	/* enable interrupts */
	__asm__ volatile("sti");
}
