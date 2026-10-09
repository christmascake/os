#ifndef ORUOS_KERNEL_INCLUDE_KERNEL_FONT_H_
#define ORUOS_KERNEL_INCLUDE_KERNEL_FONT_H_

#include <stdint.h>

#include <kernel/multiboot2.h>
#include <kernel/psf2.h>
#include <kernel/color.h>

struct font_size {
	int w, h;
};

extern const unsigned char _binary_font_cp850_8x16_psfu_start[];
extern const unsigned char _binary_font_cp850_8x16_psfu_end[];
extern const unsigned char _binary_font_cp850_8x16_psfu_size[];

// TODO:return this from a function instead of global state
// TODO:also should be perhaps stored in the tty?
extern union color32 current_font_mask[PSF2_MAX_GLYPHS][16][8];

void init_font(const uint8_t *font_start);

void print_font_info(void);
void set_font_mask(const union color32 fg, const union color32 bg);

struct font_size get_current_font_size(void);
int		 get_current_font_width(void);
int		 get_current_font_height(void);

#endif
