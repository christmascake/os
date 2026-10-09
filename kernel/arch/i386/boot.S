.set ALIGN, 1<<0

.section .multiboot
header_start:
        .long 0xE85250D6 // magic
        .long 0 // arch
        .long (header_end - header_start) // size
        .long -(0xE85250D6 + 0 + (header_end - header_start)) // checksum

        .align 8
framebuffer_tag_start:
       .word 5
       .word 0
       .long (framebuffer_tag_end - framebuffer_tag_start)
       .long 640
       .long 480
       .long 32

       .align 8
framebuffer_tag_end:
        //end flags
        .word 0
        .word 0
        .long  8
header_end:

.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.section .text
.global _start
.type _start, @function
_start:
        mov $stack_top, %esp

        pushl %ebx
        pushl %eax

        call _init
        call kernel_main

	#cli
1:	hlt
	jmp 1b

.size _start, . - _start
