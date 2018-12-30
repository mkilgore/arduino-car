#ifndef INCLUDE_COMMON_H
#define INCLUDE_COMMON_H

#define USE_PRINTF

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <inttypes.h>

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(*arr))

#endif
