#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>
#include <kernel/framebuffer.h>
#include <kernel/log.h>
#include <kernel/vga.h>
#include <kernel/font.h>

static uint16_t *const VGA_MEMORY = (uint16_t *)0xB8000;

static const size_t tty_width  = 80;
static const size_t tty_height = 25;

static size_t tty_curr_col;
static size_t tty_curr_row;

static uint8_t tty_draw_color;

static uint16_t *tty_buffer;

static struct multiboot_tag_framebuffer *fb_tag;
static uint8_t				*fb;

static struct multiboot_tag *next_tag(struct multiboot_tag **tag)
{
	// TODO: explain this madness
	*tag = (struct multiboot_tag *)((multiboot_uint8_t *)*tag +
					(((*tag)->size + 7) & ~7));
	return *tag;
}

static inline uint8_t make_color(const enum vga_color fg,
				 const enum vga_color bg)
{
	// combine 4 bits of bg with 4 bits of fg to make an 8 bit color
	return (uint8_t)(fg | (bg << 4));
}

static inline uint16_t make_entry(const unsigned char uc, const uint8_t color)
{
	// combine 8 bits of color with an 8 bit char to make a 16 bit entry
	return (uint16_t)uc | ((uint16_t)color << 8);
}

static void scroll(const int n)
{
	// TODO: map buffer to a 2d array for simplicity?

	// move all rows up one row
	// copy y+1 to y until y is the end
	for (size_t y = 1; y < tty_height; y++) {
		for (size_t x = 0; x < tty_width; x++) {
			const size_t curr = (y * tty_width) + x;
			const size_t prev = curr - tty_width;

			tty_buffer[prev] = tty_buffer[curr];
		}
	}

	// clear the last row
	for (size_t x = 0; x < tty_width; x++) {
		const size_t index = (tty_width * (tty_height - 1)) + x;
		//term->buffer[index] = vgaterm_make_entry(' ', term->draw_color);
		tty_buffer[index] = 0;
	}
}

static void handle_newline(void)
{
	tty_curr_col = 0;
	++tty_curr_row;

	// are we at the last row?
	if (tty_curr_row == tty_height) {
		// go back up 1
		tty_curr_row--;

		// we need to scroll down
		scroll(1);
	}
}

static void put_entry_at(const char c, const uint8_t color, const size_t x,
			 const size_t y)
{
	const size_t index = y * tty_width + x; // TODO: clean math
	tty_buffer[index]  = make_entry(c, color);
}

/*
        --------PUBLIC--------
*/

void init_tty(const unsigned int addr)
{
	tty_buffer = (uint16_t *)VGA_MEMORY;

	tty_curr_row = 0;
	tty_curr_col = 0;

	// set the drawing color to the default for printing
	tty_draw_color = make_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

	// clear the entire screen
	// TODO: abstract this? function pointer maybe?
	for (size_t y = 0; y < tty_height; y++) {
		for (size_t x = 0; x < tty_width; x++) {
			const size_t index = (y * tty_width) + x;

			tty_buffer[index] = make_entry(' ', tty_draw_color);
		}
	}

	struct multiboot_tag *tag = (struct multiboot_tag *)(addr + 8);

	multiboot_uint32_t pitch;
	multiboot_uint32_t width;
	multiboot_uint32_t height;
	multiboot_uint8_t  bpp;

	while (next_tag(&tag)->type != MULTIBOOT_HEADER_TAG_END) {
		if (tag->type == MULTIBOOT_TAG_TYPE_FRAMEBUFFER) {
			fb_tag = (struct multiboot_tag_framebuffer *)tag;
			fb     = (uint8_t *)fb_tag->common.framebuffer_addr;
			pitch  = fb_tag->common.framebuffer_pitch;
			width  = fb_tag->common.framebuffer_width;
			height = fb_tag->common.framebuffer_height;
			bpp    = fb_tag->common.framebuffer_bpp;
		}
	}
}

void tty_putchar(const char c)
{
	switch (c) {
	case '\n': handle_newline(); break;
	default:
		//for (unsigned int y = offrow; y < (offrow + 16); y++) {
		//	for (unsigned int x = offcol; x < (offcol + 8); x++) {
		//		uint32_t offset =
		//			y * fb_tag->common.framebuffer_pitch +
		//			x * (fb_tag->common.framebuffer_bpp /
		//			     8);
		//		*(uint32_t *)(fb + offset) = current_font_mask[(
		//			uint8_t)c][y - offrow][x - offcol]
		//						     .c;
		//	}
		//}

		framebuffer_draw_glyph_at(tty_curr_col, tty_curr_row, c);

		if (++tty_curr_col == tty_width) {
			tty_curr_col = 0;
			if (++tty_curr_row == tty_height)
				tty_curr_row = 0;
		}

		break;
	}
}

void tty_write(const char *str, const size_t n)
{
	for (size_t i = 0; i < n; i++) tty_putchar(str[i]);
}

void tty_write_string(const char *str)
{
	tty_write(str, strlen(str));
}
