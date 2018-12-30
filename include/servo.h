#ifndef INCLUDE_SERVO_H
#define INCLUDE_SERVO_H

#include <stdio.h>

void servo_init(void);

int servo_register(uint8_t volatile *port, uint8_t pin);
void servo_unregister(int);

void servo_set(int v);

#endif
