
#include "common.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "bt_gamepad.h"
#include "serial.h"


/* We implement queue as a ring buffer of messages from the serial input. As
 * characters come in, they are placed into the buffer at msg_write_idx. When
 * we recieve a newline character, we increment msg_write_idx to the next
 * value.
 *
 * Separately, msg_write_idx and msg_read_idx are checked, and if they differ,
 * then we will start processing all the messages in the queue until
 * msg_read_idx equals msg_write_idx.
 *
 * This works assuming MSG_BUF_CNT is high enough that we don't run out of
 * queue spots when getting lots of input.
 */
#define MSG_BUF_CNT 20
#define MSG_LEN 20

static volatile char msg_buf[MSG_BUF_CNT][MSG_LEN];
static volatile uint16_t msg_len[MSG_BUF_CNT];

static volatile uint16_t msg_read_idx;
static volatile uint16_t msg_write_idx;

static void handle_serial_char(char ch)
{
    /* This handles characters from the serial. Note we're called in an
     * interrupt context so we can't do the processing here and have to finish
     * up really fast. */
    if (ch == '\n') {
        if (msg_len[msg_write_idx] > 0) {
            msg_write_idx++;
            if (msg_write_idx == MSG_BUF_CNT)
                msg_write_idx = 0;
        }
    } else if (msg_len[msg_write_idx] < MSG_LEN - 1) {
        msg_buf[msg_write_idx][msg_len[msg_write_idx]] = ch;
        msg_len[msg_write_idx]++;
    }
}

struct bt_gamepad_state {
    int8_t ud_axis;
    int8_t lr_axis;

    uint8_t buttons[8];
};

static struct bt_gamepad_state gamepad_state;

static void handle_gamepad_state()
{
    /* Handle any messages in the queue */
    while (msg_read_idx != msg_write_idx) {
        /* We turn interrupts off for this part so that the serial interrupt
         * can't touch any of these variables while we work with them */
        cli();
        char tmp_msg[MSG_LEN] = { 0 };
        uint16_t len = msg_len[msg_read_idx];
        uint16_t i;

        for (i = 0; i < len; i++)
            tmp_msg[i] = msg_buf[msg_read_idx][i];

        msg_len[msg_read_idx] = 0;
        for (i = 0; i < MSG_BUF_CNT; i++)
            msg_buf[msg_read_idx][i] = 0;

        msg_read_idx++;
        if (msg_read_idx == MSG_BUF_CNT)
            msg_read_idx = 0;

        sei();

        char *stringp = tmp_msg;

        char *id = strsep(&stringp, ":");

        if (!id)
            return ;

        /*
         * The data from the BT controller is one of:
         *
         * axis:N:X:Y
         * btn:N:P
         */
        if (strcmp(id, "axis") == 0) {
            id = strsep(&stringp, ":");
            char *lr_axis = strsep(&stringp, ":");
            char *ud_axis = strsep(&stringp, ":");

            if (!id || !lr_axis || !ud_axis)
                return ;

            gamepad_state.lr_axis = atoi(lr_axis);
            gamepad_state.ud_axis = atoi(ud_axis);
        } else if (strcmp(id, "btn") == 0) {
            id = strsep(&stringp, ":");
            char *pressed = strsep(&stringp, ":");

            if (!id || !pressed)
                return ;

            int but = atoi(id);
            if (but >= ARRAY_SIZE(gamepad_state.buttons))
                return ;

            gamepad_state.buttons[but] = (*pressed == '1');
        }
    }
}


void bt_gamepad_init(void)
{
    serial_init(handle_serial_char);
}

void bt_gamepad_update_state(void)
{
    handle_gamepad_state();
}

static float normalize(int8_t v)
{
    return (float)v / 128 * 100;
}

static float abs8(float v)
{
    if (v < 0)
        return -v;

    return v;
}

void bt_gamepad_apply(struct car_state *car)
{
    /* Yay, math
     *
     * This convert the axis values into RL motor speeds. s*/
    float ud = -normalize(gamepad_state.ud_axis);
    float lr = -normalize(gamepad_state.lr_axis);

    float v = ((float)100 - abs8(lr)) * (ud / (float)100) + ud;
    float w = ((float)100 - abs8(ud)) * (lr / (float)100) + lr;

    float r = ((v + w) / 2);
    float l = ((v - w) / 2);

    if (r > 1) {
        car_state_right_motor_set(car, MOTOR_FOR);
        car_state_motor_right_speed_set(car, (uint8_t)(r * 2) + 50);
    } else if (r < -1) {
        car_state_right_motor_set(car, MOTOR_BACK);
        car_state_motor_right_speed_set(car, (uint8_t)(-r * 2) + 50);
    } else {
        car_state_right_motor_set(car, MOTOR_STOPPED);
    }

    if (l > 1) {
        car_state_left_motor_set(car, MOTOR_FOR);
        car_state_motor_left_speed_set(car, (uint8_t)(l * 2) + 50);
    } else if (l < -1) {
        car_state_left_motor_set(car, MOTOR_BACK);
        car_state_motor_left_speed_set(car, (uint8_t)(-l * 2) + 50);
    } else {
        car_state_left_motor_set(car, MOTOR_STOPPED);
    }

    int servo_degree = car->servo_degree;
    if (gamepad_state.buttons[1] && car->servo_degree < 236)
        servo_degree += 20;

    if (gamepad_state.buttons[3] && car->servo_degree > 20)
        servo_degree -= 20;

    car_state_servo_degree_set(car, servo_degree);
}
