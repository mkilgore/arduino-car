#ifndef INCLUDE_BT_GAMEPAD_H
#define INCLUDE_BT_GAMEPAD_H

#include "car_state.h"

void bt_gamepad_init(void);

void bt_gamepad_update_state(void);
void bt_gamepad_apply(struct car_state *);

#endif
