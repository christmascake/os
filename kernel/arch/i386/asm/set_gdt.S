.section .data
gdtr:
        .word 0
        .long 0

.section .text
.global set_gdt
.type set_gdt, @function
set_gdt:
        movw 4(%esp), %ax        
        movw %ax, gdtr
        movl 8(%esp), %eax
        movl %eax, gdtr+2
        lgdt gdtr
        // kernel code segment
        ljmp $0x8, $reload_data
        ret

reload_data:
        // kernel data segment
        mov $0x10, %ax
        mov %ax, %ds
        mov %ax, %es
        mov %ax, %ss
        mov %ax, %fs
        mov %ax, %gs
        ret
