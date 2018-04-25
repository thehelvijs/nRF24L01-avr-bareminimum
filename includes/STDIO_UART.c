//	https://www.gnu.org/savannah-checkouts/non-gnu/avr-libc/user-manual/group__avr__stdio.html

#ifndef F_CPU
#define F_CPU 16000000UL 
#endif

#include <stdio.h>
#include <avr/io.h>
#include "STDIO_UART.h"

#ifndef BAUD
#define BAUD 9600
#endif

#define MYUBRR (((F_CPU / (BAUD * 16UL))) - 1)

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE); 
static FILE mystdin = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

void uart_init(void)
{
    UBRR0H = MYUBRR >> 8;
    UBRR0L = MYUBRR;
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
    
    stdout = &mystdout;
    stdin  = &mystdin; 
}

// Redirect stdout to UART
int uart_putchar(char c, FILE *stream) {
	if (c == '\n') {
		uart_putchar('\r', stream);
	}
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
	return 0;
}

// Redirect stdin to UART
int uart_getchar(FILE *stream) {
	loop_until_bit_is_set(UCSR0A, RXC0);
	return UDR0;
}
