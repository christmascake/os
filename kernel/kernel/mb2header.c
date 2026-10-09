#include <assert.h>
#include <string.h>

#include <kernel/mb2header.h>
#include <kernel/multiboot2.h>
#include <kernel/framebuffer.h>

static struct {
	unsigned long magic;
	unsigned long addr;
} header_state = {
	.magic = 0,
	.addr  = 0,
};

static struct multiboot_tag *adv_tag(struct multiboot_tag **tag)
{
	// TODO: explain this madness
	*tag = (struct multiboot_tag *)((multiboot_uint8_t *)*tag +
					(((*tag)->size + 7) & ~7));
	return *tag;
}

static void check_header(void)
{
	assert(header_state.magic == MULTIBOOT2_BOOTLOADER_MAGIC);
	assert(!(header_state.addr & 7));
}

void init_mb2header(const unsigned long magic, const unsigned long addr)
{
	header_state.magic = magic;
	header_state.addr  = addr;
	check_header();

	struct multiboot_tag *tag = (struct multiboot_tag *)(addr + 8);

	while (adv_tag(&tag)->type != MULTIBOOT_TAG_TYPE_END) {
		switch (tag->type) {
		case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
			struct multiboot_tag_framebuffer *fb =
				(struct multiboot_tag_framebuffer *)tag;
			init_framebuffer(fb);
			break;
		default: break;
		}
	}
}
