
#include "common.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#include "servo.h"
#include "l298n.h"
#include "servo.h"
#include "car_state.h"

void car_state_init(void)
{
    l298n_init();
    servo_init();

    /* This turn on the PWM signals to control the motors.
     *
     * The width for the pulses are controlled via OCR0A and OCR0B */
    TCCR0A = _BV(COM0A1) | _BV(COM0B1) | _BV(WGM01) | _BV(WGM00);
    TCCR0B = _BV(CS02) | _BV(CS00);

    servo_register(&PORTD, PORTD3);
}

static void handle_motor_left(struct car_state *car)
{
    switch (car->motor_left) {
    case MOTOR_STOPPED:
        l298n_left_stop();
        break;

    case MOTOR_FOR:
        l298n_left_forward();
        break;

    case MOTOR_BACK:
        l298n_left_backward();
        break;
    }
}

static void handle_motor_right(struct car_state *car)
{
    switch (car->motor_right) {
    case MOTOR_STOPPED:
        l298n_right_stop();
        break;

    case MOTOR_FOR:
        l298n_right_forward();
        break;

    case MOTOR_BACK:
        l298n_right_backward();
        break;
    }
}

static void handle_motor_right_speed(struct car_state *car)
{
    OCR0A = car->motor_right_speed;
}

static void handle_motor_left_speed(struct car_state *car)
{
    OCR0B = car->motor_left_speed;
}

static void handle_servo_degree(struct car_state *car)
{
    servo_set(car->servo_degree);
}

void car_state_apply(struct car_state *car)
{
    if (car->motor_left_speed_changed) {
        handle_motor_left_speed(car);
        car->motor_left_speed_changed = 0;
    }

    if (car->motor_right_speed_changed) {
        handle_motor_right_speed(car);
        car->motor_right_speed_changed = 0;
    }

    if (car->motor_left_changed) {
        handle_motor_left(car);
        car->motor_left_changed = 0;
    }

    if (car->motor_right_changed) {
        handle_motor_right(car);
        car->motor_right_changed = 0;
    }

    if (car->servo_degree_changed) {
        handle_servo_degree(car);
        car->servo_degree_changed = 0;
    }
}

void car_state_left_motor_set(struct car_state *car, enum motor_dir dir)
{
    if (car->motor_left != dir) {
        car->motor_left = dir;
        car->motor_left_changed = 1;
    }
}

void car_state_right_motor_set(struct car_state *car, enum motor_dir dir)
{
    if (car->motor_right != dir) {
        car->motor_right = dir;
        car->motor_right_changed = 1;
    }
}

void car_state_servo_degree_set(struct car_state *car, uint8_t servo)
{
    if (car->servo_degree != servo) {
        car->servo_degree = servo;
        car->servo_degree_changed = 1;
    }
}

void car_state_motor_left_speed_set(struct car_state *car, uint8_t speed)
{
    if (car->motor_left_speed != speed) {
        car->motor_left_speed = speed;
        car->motor_left_speed_changed = 1;
    }
}

void car_state_motor_right_speed_set(struct car_state *car, uint8_t speed)
{
    if (car->motor_right_speed != speed) {
        car->motor_right_speed = speed;
        car->motor_right_speed_changed = 1;
    }
}

