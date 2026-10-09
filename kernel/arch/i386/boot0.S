.org 0x7c00

start:
        cli
        ljmp $0, $next_line 

next_line:
        // zero ax, ds, es
        xorw %ax, %ax
        movw %ax, %ds
        movw %ax, %es

        // real mode stack
        movw $0x0000, %ax
        movw %ax, %ss
        movw $0x7000, %sp
