#include <stdint.h>

#include <i386/include/io.h>

#define PIC1_PORT 0x20
#define PIC2_PORT 0xA0
#define PIC1_DATA (PIC1_PORT + 1)
#define PIC2_DATA (PIC2_PORT + 1)

#define PIC_EOI 0x20

#define ICW1_ICW4      0x01
#define ICW1_SINGLE    0x02
#define ICW1_FOUR_BYTE 0x04
#define ICW1_TRIGGERD  0x08
#define ICW1_INIT      0x10

#define ICW3_IRQ0_HAS_SLAVE 0x01
#define ICW3_IRQ1_HAS_SLAVE 0x02
#define ICW3_IRQ2_HAS_SLAVE 0x04
#define ICW3_IRQ3_HAS_SLAVE 0x08
#define ICW3_IRQ4_HAS_SLAVE 0x10
#define ICW3_IRQ5_HAS_SLAVE 0x20
#define ICW3_IRQ6_HAS_SLAVE 0x40
#define ICW3_IRQ7_HAS_SLAVE 0x80

#define ICW4_8086	0x01
#define ICW4_AUTO_EOI	0x02
#define ICW4_SLAVE_BUF	0x08
#define ICW4_MASTER_BUF 0x0C
#define ICW4_SFNM	0x10

#define CASCADE_IRQ 2

static inline uint16_t get_irq_reg(const int ocw3)
{
}

void PIC_send_EOI(const uint8_t irq)
{
	if (irq >= 8)
		io_outb(PIC2_PORT, PIC_EOI);
	io_outb(PIC1_PORT, PIC_EOI);
}

void PIC_remap(const uint8_t offset1, const uint8_t offset2)
{
	io_outb(PIC1_PORT, ICW1_INIT | ICW1_ICW4);
	io_wait();
	io_outb(PIC2_PORT, ICW1_INIT | ICW1_ICW4);
	io_wait();
	io_outb(PIC1_DATA, offset1);
	io_wait();
	io_outb(PIC2_DATA, offset2);
	io_wait();
	io_outb(PIC1_DATA, ICW3_IRQ2_HAS_SLAVE); // don't understand this
	io_wait();
	io_outb(PIC2_DATA, 2); // or this
	io_wait();
	io_outb(PIC1_DATA, ICW4_8086);
	io_wait();
	io_outb(PIC2_DATA, ICW4_8086);
	io_wait();

	/* unmask */
	io_outb(PIC1_DATA, 0);
	io_outb(PIC2_DATA, 0);
}
