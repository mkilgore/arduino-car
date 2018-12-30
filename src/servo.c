
#include "common.h"

#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include "servo.h"

/* This is only half-implemented at the moment. It *should* support more the
 * one servo, however it currently only does one.
 *
 * The servo waveform is a 50hz signal (20ms long), where the first ~1ms to ~2.5ms
 * control the location of the servo.
 *
 * The signal starts high. If it is held high for only 1ms (and then low for
 * the rest of the 19ms), then the servo will be located at the very beginning
 * of it's range.
 *
 * If it is held high for about 2.5ms, then the servo will be located at the
 * very end.
 *
 *
 * We cannot easily generate this signal just via PWM (The signal is too slow,
 * and on/off is too short), so we instead generate it by hand triggering the
 * timer interrupt and then toggling the outputs manually.
 */

/* We make use of TIMER1, which is the only 16-bit timer. This is important
 * since the 16-bit timer can count-up to a 16-bit value, and thus can count to
 * much larger values then the 8-bit timers (32785 vs 255).
 *
 * This prescaler gives us a pretty good speed and accuracy.
 */
#define TIMER1_PRESCALER 8

#define TICKS_PER_US (F_CPU / 1000000UL)

/* Convert timer ticks to microseconds */
static inline unsigned long ticks_to_us(unsigned long ticks)
{
    return (ticks * TIMER1_PRESCALER) / TICKS_PER_US;
}

static inline unsigned long us_to_ticks(unsigned long us)
{
    return (TICKS_PER_US * us) / TIMER1_PRESCALER;
}

/* These constants give us about a 50hz refresh rate, which is the max. It's
 * set to be a little lower then 50hz to ensure we're not too fast.
 *
 * What is the refresh period? It's a period where we don't change the state of
 * any servos. It's only necessary because the timer is too slow to count out
 * 50hz. The us_to_ticks() conversion of 20ms would overflow the 16-bit target
 * value. So we get around that by counting first to the SERVO_PREIOD_TICKS
 * value, and then count the SERVO_REFRESH_PERIOD_TICKS value. */
#define SERVO_REFRESH_PERIOD_TICKS   us_to_ticks(11000UL)
#define SERVO_PERIOD_TICKS           us_to_ticks(10000UL)

#define SERVO_DUTY_CYCLE_MIN_TICKS   us_to_ticks(500UL)
#define SERVO_DUTY_CYCLE_PULSE_TICKS us_to_ticks(2000UL)

struct servo {
    uint8_t volatile *port;
    uint8_t pin;

    uint16_t duty_cycle_ticks;
    uint8_t is_on;
};

static struct servo servos[1];

/* We only have one servo, so this just goes back and forth between 0, or -1 for the refresh period */
static int next_servo = 0;

ISR(TIMER1_COMPA_vect)
{
    if (next_servo == -1) {
        OCR1A = SERVO_REFRESH_PERIOD_TICKS;
        next_servo = 0;
    } else if (servos[0].port) {
        if (servos[0].is_on) {
            *servos[0].port &= ~_BV(servos[0].pin);
            OCR1A = SERVO_PERIOD_TICKS - servos[0].duty_cycle_ticks;
            servos[0].is_on = 0;
            next_servo = -1;
        } else {
            *servos[0].port |= _BV(servos[0].pin);
            servos[0].is_on = 1;
            OCR1A = servos[0].duty_cycle_ticks;
        }
    } else {
        OCR1A = SERVO_PERIOD_TICKS;
    }

    TCNT1 = 0;
}

void servo_init(void)
{
    /* This sets timer 1 to count up, with a prescaler of 8.
     * When it hits OCR1A, it will trigger an interrupt */
    TCCR1A = 0;
    TCCR1B = _BV(CS11);
    TCNT1 = 0;

    /* Tell it to trigger an interrupt when TCNT1 hits OCR0A */
    TIFR1 |= _BV(OCF1A);
    TIMSK1 |= _BV(OCIE1A);

    OCR1A = SERVO_PERIOD_TICKS;
}

int servo_register(uint8_t volatile *port, uint8_t pin)
{
    uint8_t sreg = SREG;
    cli();

    servos[0].port = port;
    servos[0].pin = pin;
    servos[0].is_on = 0;
    servos[0].duty_cycle_ticks = SERVO_DUTY_CYCLE_MIN_TICKS;

    printf("PIN: %d, ticks: %d\n", pin, servos[0].duty_cycle_ticks);

    OCR1A = servos[0].duty_cycle_ticks;

    SREG = sreg;
    return 1;
}

void servo_unregister(int s)
{
    uint8_t sreg = SREG;
    cli();

    servos[0].port = NULL;

    SREG = sreg;
}

void servo_set(int degree)
{
    uint8_t sreg = SREG;
    cli();

    servos[0].duty_cycle_ticks = SERVO_DUTY_CYCLE_MIN_TICKS + degree * (SERVO_DUTY_CYCLE_PULSE_TICKS / 256);

    SREG = sreg;
}

