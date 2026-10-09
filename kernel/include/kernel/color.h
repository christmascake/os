#ifndef ORUOS_KERNEL_INCLUDE_KERNEL_COLOR_H_
#define ORUOS_KERNEL_INCLUDE_KERNEL_COLOR_H_

#include <stdint.h>

union color32 {
	uint32_t c;
	struct {
		uint8_t b, g, r, a;
	};
};

extern const union color32 color32_WHITE;
extern const union color32 color32_BLACK;

#endif
