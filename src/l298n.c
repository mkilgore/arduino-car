
#include "common.h"

#include <avr/io.h>

#include "l298n.h"

/*
 * The L298N controls the motors.
 *
 * It's a dual H-Bridge controller, with two inputs per bridge (To drive each
 * pin of the motor high or low) and an enable/disable pin for each.
 *
 * The enable/disable pin can be controlled via PWM to vary the speed.
 */

void l298n_init(void)
{
    L298N_ENA_DDR |= _BV(L298N_ENA_PIN);
    L298N_ENB_DDR |= _BV(L298N_ENB_PIN);
    L298N_RIGHT_BACK_DDR |= _BV(L298N_RIGHT_BACK_PIN);
    L298N_RIGHT_FOR_DDR |= _BV(L298N_RIGHT_FOR_PIN);
    L298N_LEFT_BACK_DDR |= _BV(L298N_LEFT_BACK_PIN);
    L298N_LEFT_FOR_DDR |= _BV(L298N_LEFT_FOR_PIN);

    L298N_ENA_PORT |= _BV(L298N_ENA_PIN);
    L298N_ENB_PORT |= _BV(L298N_ENB_PIN);
}

void l298n_left_enable(void)
{
    L298N_ENA_PORT |= _BV(L298N_ENA_PIN);
}

void l298n_left_disable(void)
{
    L298N_ENA_PORT &= ~_BV(L298N_ENA_PIN);
}

void l298n_left_toggle(void)
{
    L298N_ENA_PORT ^= _BV(L298N_ENA_PIN);
}

void l298n_left_stop(void)
{
    L298N_LEFT_BACK_PORT &= ~_BV(L298N_LEFT_BACK_PIN);
    L298N_LEFT_FOR_PORT &= ~_BV(L298N_LEFT_FOR_PIN);
}

void l298n_left_forward(void)
{
    L298N_LEFT_BACK_PORT &= ~_BV(L298N_LEFT_BACK_PIN);
    L298N_LEFT_FOR_PORT |= _BV(L298N_LEFT_FOR_PIN);
}

void l298n_left_backward(void)
{
    L298N_LEFT_BACK_PORT |= _BV(L298N_LEFT_BACK_PIN);
    L298N_LEFT_FOR_PORT &= ~_BV(L298N_LEFT_FOR_PIN);
}

void l298n_right_enable(void)
{
    L298N_ENB_PORT |= _BV(L298N_ENB_PIN);
}

void l298n_right_disable(void)
{
    L298N_ENB_PORT &= ~_BV(L298N_ENB_PIN);
}

void l298n_right_toggle(void)
{
    L298N_ENB_PORT ^= _BV(L298N_ENB_PIN);
}

void l298n_right_stop(void)
{
    L298N_RIGHT_BACK_PORT &= ~_BV(L298N_RIGHT_BACK_PIN);
    L298N_RIGHT_FOR_PORT &= ~_BV(L298N_RIGHT_FOR_PIN);
}

void l298n_right_forward(void)
{
    L298N_RIGHT_BACK_PORT &= ~_BV(L298N_RIGHT_BACK_PIN);
    L298N_RIGHT_FOR_PORT |= _BV(L298N_RIGHT_FOR_PIN);
}

void l298n_right_backward(void)
{
    L298N_RIGHT_BACK_PORT |= _BV(L298N_RIGHT_BACK_PIN);
    L298N_RIGHT_FOR_PORT &= ~_BV(L298N_RIGHT_FOR_PIN);
}
