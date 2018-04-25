//	https://www.gnu.org/savannah-checkouts/non-gnu/avr-libc/user-manual/group__avr__stdio.html

#ifndef STDIO_UART_H_
#define STDIO_UART_H_

void uart_init(void);
int uart_putchar(char c, FILE *stream); 
int uart_getchar(FILE *stream);

#endif /* STDIO_UART_H_ */
