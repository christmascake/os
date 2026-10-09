#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include <kernel/font.h>
#include <kernel/psf2.h>

static struct {
	const struct psf2_header *header;
	uint8_t			 *bitmap_start;

} font_state = {
	.header	      = NULL,
	.bitmap_start = NULL,
};

union color32 current_font_mask[PSF2_MAX_GLYPHS][16][8];

static void check_header(void)
{
	const auto reported_magic = font_state.header->magic[0] << 24 |
				    font_state.header->magic[1] << 16 |
				    font_state.header->magic[2] << 8 |
				    font_state.header->magic[3];

	assert(reported_magic == PSF2_MAGIC_BE);
	assert(font_state.header->version == PSF2_MAXVERSION);
	assert(font_state.header->flags == PSF2_HAS_UNICODE_TABLE);
}

void init_font(const unsigned char *font_start)
{
	font_state.header = (struct psf2_header *)font_start;
	font_state.bitmap_start =
		(uint8_t *)(font_start + font_state.header->headersize);

	/* print any psf2 header errors to the log */
	check_header();

	/* default mask is white text on black background */
	set_font_mask(color32_WHITE, color32_BLACK);
}

/*
 * INFO:set the color for every glyph in the font, might be expensive if we are
 * rapidly changing color for each character we print e.g rainbow text. (?)
 */

// TODO: add safety, bpp, resolution, endian check
void set_font_mask(const union color32 fg, const union color32 bg)
{
	const auto total_glyphs = font_state.header->length;
	const auto height	= font_state.header->height;
	const auto width	= font_state.header->width;

	for (unsigned int i = 0; i < total_glyphs; i++) {
		const auto glyph = (font_state.bitmap_start +
				    (font_state.header->charsize * i));

		for (unsigned int j = 0; j < height; j++) {
			const auto row = glyph[j];

			for (unsigned int k = 0; k < width; k++) {
				current_font_mask[i][j][k] =
					((row >> (7u - k)) & 1u) ? fg : bg;
			}
		}
	}
}

void print_font_info(void)
{
	printf("version: %d\n", font_state.header->version);
	printf("headersize: %d\n", font_state.header->headersize);
	printf("flags: %d\n", font_state.header->flags);
	printf("length: %d\n", font_state.header->length);
	printf("charsize: %d\n", font_state.header->charsize);
	printf("height: %d\n", font_state.header->height);
	printf("width: %d\n", font_state.header->width);
}

struct font_size get_current_font_size(void)
{
	struct font_size ret;

	ret.w = font_state.header->width;
	ret.h = font_state.header->height;

	return ret;
}

int get_current_font_width(void)
{
	return font_state.header->width;
}

int get_current_font_height(void)
{
	return font_state.header->height;
}
