.extern idt_cpu_exception_err
.extern idt_cpu_exception_no_err

.macro isr_err_stub num
.globl isr_stub_\num
isr_stub_\num:
    pushl $\num
    call isr_cpu_exception_err
    iret
    .endm

    .macro isr_no_err_stub num
    .globl isr_stub_\num
isr_stub_\num:
    pusha
    pushl $0
    pushl $\num
    call isr_cpu_exception_no_err
    addl $8, %esp
    iret
    .endm

isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31

.globl isr_stub_table
.type  isr_stub_table, @object
.align 4
isr_stub_table:
    .altmacro
    .macro isr_stub_table_entries count
        .if \count
            isr_stub_table_entries %(\count-1)
        .endif
        .long isr_stub_\count
    .endm
    isr_stub_table_entries 31
    .size isr_stub_table, . - isr_stub_table
