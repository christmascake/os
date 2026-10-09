.section .init
	/* crtend.o .init section */
	popl %ebp
	ret

.section .fini
	/* crtend.o .fini section */
	popl %ebp
	ret
