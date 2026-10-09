#ifndef ORUOS_KERNEL_INCLUDE_KERNEL_LOG_H_
#define ORUOS_KERNEL_INCLUDE_KERNEL_LOG_H_

#ifdef __arch_i386

#include <stdio.h>
#include <i386/include/serial.h>

#define printk(fmt, ...) fctprintf(serial_fctputchar, NULL, fmt, ##__VA_ARGS__)

#endif
#endif
