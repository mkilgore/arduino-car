
#include "common.h"

#include <stdio.h>

#include <avr/interrupt.h>
#include <avr/io.h>

#include "debug_serial.h"
#include "serial.h"
#include "twi_master.h"
#include "snes_classic.h"
#include "ultrasonic.h"
#include "car_state.h"
#include "bt_gamepad.h"

static struct snes_classic_state snes_state;

static struct car_state car_state = {
    .motor_left_speed_changed = 1,
    .motor_right_speed_changed = 1,
    .motor_left_changed = 1,
    .motor_right_changed = 1,
    .servo_degree_changed = 1,

    .motor_left_speed = 200,
    .motor_right_speed = 200,
    .servo_degree = 128,
    .motor_left = MOTOR_STOPPED,
    .motor_right = MOTOR_STOPPED,
};

void snes_controller_handle_state(struct snes_classic_state *state, struct car_state *car)
{
    if (snes_state.up_pressed) {
        car_state_left_motor_set(car, MOTOR_FOR);
        car_state_right_motor_set(car, MOTOR_FOR);
    } else if (snes_state.down_pressed) {
        car_state_left_motor_set(car, MOTOR_BACK);
        car_state_right_motor_set(car, MOTOR_BACK);
    } else if (snes_state.left_pressed) {
        car_state_left_motor_set(car, MOTOR_BACK);
        car_state_right_motor_set(car, MOTOR_FOR);
    } else if (snes_state.right_pressed) {
        car_state_left_motor_set(car, MOTOR_FOR);
        car_state_right_motor_set(car, MOTOR_BACK);
    } else {
        car_state_left_motor_set(car, MOTOR_STOPPED);
        car_state_right_motor_set(car, MOTOR_STOPPED);
    }

    int new_motor_speed = car->motor_left_speed;

    if (snes_state.x_pressed && car->motor_left_speed < 250)
        new_motor_speed += 10;

    if (snes_state.y_pressed && car->motor_left_speed > 150)
        new_motor_speed -= 10;

    car_state_motor_left_speed_set(car, new_motor_speed);
    car_state_motor_right_speed_set(car, new_motor_speed);

    uint8_t new_servo_degree = car->servo_degree;

    if (snes_state.r_pressed && car->servo_degree > 20)
        new_servo_degree -= 20;

    if (snes_state.l_pressed && car->servo_degree < 230)
        new_servo_degree += 20;

    car_state_servo_degree_set(car, new_servo_degree);
}

int main(void)
{
    debug_serial_init();
    bt_gamepad_init();
    twi_master_init();
    car_state_init();
    sei();

    DDRD |= _BV(DDD3);
    PORTD &= ~_BV(PORTD3);

    /* For Ultrasonic Sensor */
    DDRC |= _BV(DDC5);
    DDRC &= ~_BV(DDC4);
    ultrasonic_init();

    int snes_controller_attached = !snes_classic_init();

    int i = 0;
    while (1) {
        if (snes_controller_attached) {
            snes_classic_read_state(&snes_state);
            snes_controller_handle_state(&snes_state, &car_state);
        } else {
            bt_gamepad_update_state();
            bt_gamepad_apply(&car_state);
        }

        car_state_apply(&car_state);

        if (!i)
            ultrasonic_read_distance();

        i++;
        if (i == 15)
            i = 0;

        _delay_ms(16);
    }

    return 0;
}

