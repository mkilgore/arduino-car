
#include "common.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/setbaud.h>

#include "serial.h"

/* Hardware serial
 *
 * When we recieve a char, we call the set callback */

static void (*serial_callback) (char);

ISR(USART_RX_vect)
{
    char c = UDR0;
    serial_callback(c);
}

void serial_send_char(char c)
{
	while(!(UCSR0A & (1<<UDRE0)))
        ; // wait until sending is possible

	UDR0 = c; // output character saved in c
}

void serial_init(void (*char_callback) (char c))
{
    /* We're using the setbaud.h magic to calculate the baudrate flag values.
     *
     * It is derived from the BAUD macro, which is provided via the Makefile */
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~_BV(U2X0);
#endif

    serial_callback = char_callback;

    /* Turn on RX and TX, as well as the RX interrupt */
	UCSR0B |= _BV(TXEN0) | _BV(RXEN0) | _BV(RXCIE0);
	UCSR0C |= _BV(UCSZ01) | _BV(UCSZ00);
}

