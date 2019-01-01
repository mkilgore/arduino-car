
#include "common.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

#include "serial.h"
#include "ultrasonic.h"

#define TICKS_PER_US (F_CPU / 1000000UL)

static volatile uint16_t overflow_count;

ISR(TIMER2_OVF_vect)
{
    overflow_count++;
}

static inline uint16_t ms_to_cm(unsigned long duration)
{
    return (uint16_t)((duration / 2) / 29.1);
}

static FILE *serial_out;

static int serial_write(char c, FILE *f)
{
    serial_send_char(c);
    return c;
}

void ultrasonic_init(void)
{
    ULTRASONIC_TRIG_PORT &= ~_BV(ULTRASONIC_TRIG_PIN_N);

    TCCR2B = 0;
    TCCR2A = 0;
    TIMSK2 |= _BV(TOIE2);

    serial_out = fdevopen(serial_write, NULL);
}

uint16_t ultrasonic_read_distance(void)
{
    TCNT2 = 0;
    TIFR2 |= _BV(TOV2);

    overflow_count = 0;

    ULTRASONIC_TRIG_PORT |= _BV(ULTRASONIC_TRIG_PIN_N);
    _delay_us(10);
    ULTRASONIC_TRIG_PORT &= ~_BV(ULTRASONIC_TRIG_PIN_N);


    /* Wait for rising edge */
    while (!(ULTRASONIC_ECHO_PIN & _BV(ULTRASONIC_ECHO_PIN_N)))
        ;

    /* Turn on the timer with 128 prescaler */
    TCCR2B |= _BV(CS20) | _BV(CS22);

    while (ULTRASONIC_ECHO_PIN & _BV(ULTRASONIC_ECHO_PIN_N))
        ;

    TCCR2B = 0;

    uint16_t orig_overflow = overflow_count;
    uint16_t orig_count = TCNT2;
    uint16_t count = orig_count + overflow_count * 256;

    double us = ((double)count * 128) / TICKS_PER_US;

    double distance = us / 58.0;

    char usbuf[30];
    char buf[30];

    dtostrf(distance, 3, 3, buf);
    dtostrf(us, 3, 3, usbuf);

    fprintf(serial_out, "ult:%d:%d:%s:%s\n", orig_overflow, count, usbuf, buf);

    return 0;
}

