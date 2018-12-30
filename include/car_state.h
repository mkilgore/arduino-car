#ifndef INCLUDE_CAR_STATE_H
#define INCLUDE_CAR_STATE_H

enum motor_dir {
    MOTOR_STOPPED,
    MOTOR_FOR,
    MOTOR_BACK,
};

struct car_state {
    uint8_t motor_left_speed_changed :1;
    uint8_t motor_right_speed_changed :1;
    uint8_t motor_left_changed :1;
    uint8_t motor_right_changed :1;
    uint8_t servo_degree_changed :1;

    uint8_t motor_left_speed;
    uint8_t motor_right_speed;
    uint8_t servo_degree;

    enum motor_dir motor_left;
    enum motor_dir motor_right;
};

void car_state_init(void);
void car_state_apply(struct car_state *);
void car_state_left_motor_set(struct car_state *car, enum motor_dir dir);
void car_state_right_motor_set(struct car_state *car, enum motor_dir dir);
void car_state_servo_degree_set(struct car_state *car, uint8_t servo);
void car_state_motor_left_speed_set(struct car_state *car, uint8_t speed);
void car_state_motor_right_speed_set(struct car_state *car, uint8_t speed);

#endif
