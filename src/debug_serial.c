
#include "common.h"

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#include "debug_serial.h"

#define SERIAL_PORT PORTC
#define SERIAL_PIN  PORTC2

/* A delay of 103us gives us right around 9600 baud */
#define BAUD_DELAY 103

/* This is a bitbangged TX-only UART, just for debugging output.
 *
 * The hardware serial is used by other parts of the system. */
static int serial_putc(char c, FILE *f)
{
    /* Send start bit */
    SERIAL_PORT &= ~_BV(SERIAL_PIN);
    _delay_us(BAUD_DELAY);

    int i;
    for (i = 0; i < 8; i++, c >>= 1) {
        if (c & 1)
            SERIAL_PORT |= _BV(SERIAL_PIN);
        else
            SERIAL_PORT &= ~_BV(SERIAL_PIN);

        _delay_us(BAUD_DELAY);
    }

    /* Send stop bit */
    SERIAL_PORT |= _BV(SERIAL_PIN);
    _delay_us(BAUD_DELAY);

    return c;
}

void debug_serial_init(void)
{
    DDRC |= _BV(DDC2);

    SERIAL_PORT |= _BV(SERIAL_PIN);

#ifdef USE_PRINTF
    fdevopen(&serial_putc, NULL);
#endif
}


