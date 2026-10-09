#ifndef ORUOS_KERNEL_ARCH_I386_INCLUDE_PIC_H_
#define ORUOS_KERNEL_ARCH_I386_INCLUDE_PIC_H_

#include <stdint.h>

void PIC_remap(const uint8_t offset1, const uint8_t offset2);
void PIC_send_EOI(const uint8_t irq);
//uint16_t PIC_get_irr(void);
//uint16_t PIC_get_isr(void);

#endif
