.extern irq_handle_common

.macro irq_stub num
.globl irq_stub_\num
irq_stub_\num:
      pushal
      movl %esp, %eax # save pointer to the registers
      pushl $\num     # push the irq number, second c argument
      pushl %eax      # push pointer to the registers, first c argument
      call irq_handle_common
      addl $8, %esp
      popal
      iret
.endm

irq_stub 0
irq_stub 1
irq_stub 2
irq_stub 3
irq_stub 4
irq_stub 5
irq_stub 6
irq_stub 7
irq_stub 8
irq_stub 9
irq_stub 10
irq_stub 11
irq_stub 12
irq_stub 13
irq_stub 14
irq_stub 15

.globl irq_stub_table
.type irq_stub_table, @object
.align 4
irq_stub_table:
        .long irq_stub_0
        .long irq_stub_1
        .long irq_stub_2
        .long irq_stub_3
        .long irq_stub_4
        .long irq_stub_5
        .long irq_stub_6
        .long irq_stub_7
        .long irq_stub_8
        .long irq_stub_9
        .long irq_stub_10
        .long irq_stub_11
        .long irq_stub_12
        .long irq_stub_13
        .long irq_stub_14
        .long irq_stub_15
.size irq_stub_table, . - irq_stub_table
