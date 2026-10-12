#ifndef ORUOS_KERNEL_INCLUDE_KERNEL_KEYBOARD_H_
#define ORUOS_KERNEL_INCLUDE_KERNEL_KEYBOARD_H_

void	      keyboard_set_buf(unsigned char c);
void	      keyboard_clear_buf(void);
unsigned char keyboard_poll_buffer(void);

#endif
