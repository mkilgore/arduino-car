
#include "common.h"

#include "ultrasonic.h"

static inline uint16_t ms_to_cm(unsigned long duration)
{
    return (uint16_t)((duration / 2) / 29.1);
}

void ultrasonic_init(void)
{
    ULTRASONIC_TRIG_PORT &= ~_BV(ULTRASONIC_TRIG_PIN_N);
}

uint16_t ultrasonic_read_distance(void)
{
    ULTRASONIC_TRIG_PORT |= _BV(ULTRASONIC_TRIG_PIN_N);
    _delay_us(10);
    ULTRASONIC_TRIG_PORT &= ~_BV(ULTRASONIC_TRIG_PIN_N);

    /* TODO: Read echo */

    return 0;
}

