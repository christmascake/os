#include <stdio.h>
#include <string.h>

#include <kernel/framebuffer.h>
#include <kernel/font.h>
#include <kernel/log.h>

static struct {
	struct multiboot_tag_framebuffer *info;
	uint8_t				 *buffer;

} fb_state = {
	.info	= NULL,
	.buffer = NULL,
};

void framebuffer_draw_glyph_at(const int x, const int y,
			       const unsigned char glyph)
{
	/* offsets based on character position */
	const auto w	  = get_current_font_width();
	const auto h	  = get_current_font_height();
	const auto offcol = x * w;
	const auto offrow = y * h;

	//uint32_t *foo = (uint32_t *)fb_state.info->common.framebuffer_addr;

	//for (int i = 0; i < 100; i++) {
	//	*foo = 123;
	//	foo += 4;
	//}

	for (auto i = offrow; i < (offrow + h); i++) {
		for (auto j = offcol; j < (offcol + w); j++) {
			uint32_t offset =
				i * fb_state.info->common.framebuffer_pitch +
				j * (fb_state.info->common.framebuffer_bpp / 8);

			*(uint32_t *)(fb_state.buffer + offset) =
				current_font_mask[glyph][i - offrow][j - offcol]
					.c;
		}
	}
}

void init_framebuffer(struct multiboot_tag_framebuffer *info)
{
	fb_state.info	= info;
	fb_state.buffer = (uint8_t *)info->common.framebuffer_addr;
}
