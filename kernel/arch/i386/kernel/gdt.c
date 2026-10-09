#include <assert.h>
#include <stdio.h>
#include <stdint.h>

#include <i386/include/gdt.h>

// Each define here is for a specific flag in the descriptor.
// Refer to the intel documentation for a description of what each one does.
#define SEG_DESCTYPE(x) \
	((x) << 0x04) // Descriptor type (0 for system, 1 for code/data)
#define SEG_PRES(x) ((x) << 0x07) // Present
#define SEG_SAVL(x) ((x) << 0x0C) // Available for system use
#define SEG_LONG(x) ((x) << 0x0D) // Long mode
#define SEG_SIZE(x) ((x) << 0x0E) // Size (0 for 16-bit, 1 for 32)
#define SEG_GRAN(x) \
	((x) << 0x0F) // Granularity (0 for 1B - 1MB, 1 for 4KB - 4GB)
#define SEG_PRIV(x) (((x) & 0x03) << 0x05) // Set privilege level (0 - 3)

#define SEG_DATA_RD	   0x00 // Read-Only
#define SEG_DATA_RDA	   0x01 // Read-Only, accessed
#define SEG_DATA_RDWR	   0x02 // Read/Write
#define SEG_DATA_RDWRA	   0x03 // Read/Write, accessed
#define SEG_DATA_RDEXPD	   0x04 // Read-Only, expand-down
#define SEG_DATA_RDEXPDA   0x05 // Read-Only, expand-down, accessed
#define SEG_DATA_RDWREXPD  0x06 // Read/Write, expand-down
#define SEG_DATA_RDWREXPDA 0x07 // Read/Write, expand-down, accessed
#define SEG_CODE_EX	   0x08 // Execute-Only
#define SEG_CODE_EXA	   0x09 // Execute-Only, accessed
#define SEG_CODE_EXRD	   0x0A // Execute/Read
#define SEG_CODE_EXRDA	   0x0B // Execute/Read, accessed
#define SEG_CODE_EXC	   0x0C // Execute-Only, conforming
#define SEG_CODE_EXCA	   0x0D // Execute-Only, conforming, accessed
#define SEG_CODE_EXRDC	   0x0E // Execute/Read, conforming
#define SEG_CODE_EXRDCA	   0x0F // Execute/Read, conforming, accessed

#define GDT_CODE_PL0                                                \
	SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | SEG_LONG(0) | \
		SEG_SIZE(1) | SEG_GRAN(1) | SEG_PRIV(0) | SEG_CODE_EXRD

#define GDT_DATA_PL0                                                \
	SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | SEG_LONG(0) | \
		SEG_SIZE(1) | SEG_GRAN(1) | SEG_PRIV(0) | SEG_DATA_RDWR

#define GDT_CODE_PL3                                                \
	SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | SEG_LONG(0) | \
		SEG_SIZE(1) | SEG_GRAN(1) | SEG_PRIV(3) | SEG_CODE_EXRD

#define GDT_DATA_PL3                                                \
	SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | SEG_LONG(0) | \
		SEG_SIZE(1) | SEG_GRAN(1) | SEG_PRIV(3) | SEG_DATA_RDWR

#define CORRECT_KERNEL_DATA_OFFSET 0x10
#define CORRECT_KERNEL_CODE_OFFSET 0x8

/* INFO: defined in set_gdt.S */
extern void set_gdt(uint16_t limit, uint32_t base);

static struct {
	uint64_t null_descriptor;
	uint64_t kernel_code;
	uint64_t kernel_data;
	uint64_t user_code;
	uint64_t user_data;
} __attribute__((packed)) gdt_table = {};

/*
        full64:
        bits:   63..56   55..52   51..48   47..40   39..32    31..16     15..0
        fields: B31_24   FLAGS    L19_16   ACC      B23_16    B15_0     L15_0
        
        ASCII:
        [ B31_24 ] [FLAGS] [L19_16] [  ACC   ] [ B23_16 ] [   B15_0   ] [  L15_0  ]
        ( 8 bits)  (4)     (4)      (8 bits)    (8)        (16 bits)    (16 bits)
        
*/

static uint64_t create_descriptor(uint32_t base, uint32_t limit, uint16_t flag)
{
	uint64_t descriptor;

	// Create the high 32 bit segment
	descriptor = limit & 0x000F0000; // set limit bits 19:16
	descriptor |=
		(flag << 8) &
		0x00F0FF00; // set type, p, dpl, s, g, d/b, l and avl fields
	descriptor |= (base >> 16) & 0x000000FF; // set base bits 23:16
	descriptor |= base & 0xFF000000; // set base bits 31:24

	// Shift by 32 to allow for low part of segment
	descriptor <<= 32;

	// Create the low 32 bit segment
	descriptor |= base << 16; // set base bits 15:0
	descriptor |= limit & 0x0000FFFF; // set limit bits 15:0

	return descriptor;
}

static void check_segments(void)
{
	uint16_t cs, ds, es, fs, gs, ss;

	__asm__ volatile("mov %%cs, %0" : "=r"(cs));
	__asm__ volatile("mov %%ds, %0" : "=r"(ds));
	__asm__ volatile("mov %%es, %0" : "=r"(es));
	__asm__ volatile("mov %%fs, %0" : "=r"(fs));
	__asm__ volatile("mov %%gs, %0" : "=r"(gs));
	__asm__ volatile("mov %%ss, %0" : "=r"(ss));

	assert(cs == CORRECT_KERNEL_CODE_OFFSET);
	assert(ds == CORRECT_KERNEL_DATA_OFFSET);
	assert(es == CORRECT_KERNEL_DATA_OFFSET);
	assert(fs == CORRECT_KERNEL_DATA_OFFSET);
	assert(gs == CORRECT_KERNEL_DATA_OFFSET);
	assert(ss == CORRECT_KERNEL_DATA_OFFSET);
}

void init_gdt(void)
{
	gdt_table.null_descriptor = create_descriptor(0, 0, 0);
	gdt_table.kernel_code	  = create_descriptor(0, 0xFFFFF, GDT_CODE_PL0);
	gdt_table.kernel_data	  = create_descriptor(0, 0xFFFFF, GDT_DATA_PL0);
	gdt_table.user_data	  = create_descriptor(0, 0xFFFFF, GDT_DATA_PL3);
	gdt_table.user_code	  = create_descriptor(0, 0xFFFFF, GDT_CODE_PL3);

	set_gdt((uint16_t)(sizeof(gdt_table) - 1), (uint32_t)&gdt_table);

	check_segments();
}
