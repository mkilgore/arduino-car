#ifndef INCLUDE_ULTRASONIC_H
#define INCLUDE_ULTRASONIC_H

#define ULTRASONIC_TRIG_PORT  PORTC
#define ULTRASONIC_TRIG_PIN_N PORTC5

#define ULTRASONIC_ECHO_PIN   PINC
#define ULTRASONIC_ECHO_PIN_N PINC4

void ultrasonic_init(void);

/* Returned value is in cm */
uint16_t ultrasonic_read_distance(void);

#endif
