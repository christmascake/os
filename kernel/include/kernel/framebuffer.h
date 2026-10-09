#ifndef ORUOS_KERNEL_INCLUDE_KERNEL_FRAMEBUFFER_H_
#define ORUOS_KERNEL_INCLUDE_KERNEL_FRAMEBUFFER_H_

#include <kernel/multiboot2.h>
#include <kernel/mb2header.h>

void init_framebuffer(struct multiboot_tag_framebuffer *info);

/**
 * @brief draw glyph to the framebuffer using the current font mask
 *
 * @param x coordinate in characters
 * @param y coordinate in characters
 * @param glyph glyph to draw
 */
void framebuffer_draw_glyph_at(const int x, const int y,
			       const unsigned char glyph);

#endif
