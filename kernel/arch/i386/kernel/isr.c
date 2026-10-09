#include <stdint.h>
#include <stdio.h>

#include <kernel/log.h>

__attribute__((__noreturn__)) void isr_cpu_exception_err(uint32_t num,
							 uint32_t errcode)
{
	printk("isr_cpu_exception_err(): isr_stub_%u: err:%u\n", num, errcode);
	__asm__ volatile("cli; hlt");
	__builtin_unreachable();
}

__attribute__((__noreturn__)) void isr_cpu_exception_no_err(uint32_t num,
							    uint32_t errcode)
{
	printk("isr_cpu_exception_no_err(): isr_stub_%u: err:%u\n", num,
	       errcode);
	__asm__ volatile("cli; hlt");
	__builtin_unreachable();
}
