#ifndef INCLUDE_L298N_H
#define INCLUDE_L298N_H

#define L298N_ENA_DDR  DDRD
#define L298N_ENA_PORT PORTD
#define L298N_ENA_PIN  PORTD6

#define L298N_ENB_DDR  DDRD
#define L298N_ENB_PORT PORTD
#define L298N_ENB_PIN  PORTD5

#define L298N_LEFT_FOR_DDR  DDRD
#define L298N_LEFT_FOR_PORT PORTD
#define L298N_LEFT_FOR_PIN  PORTD7

#define L298N_LEFT_BACK_DDR  DDRB
#define L298N_LEFT_BACK_PORT PORTB
#define L298N_LEFT_BACK_PIN  PORTB0

#define L298N_RIGHT_BACK_DDR  DDRB
#define L298N_RIGHT_BACK_PORT PORTB
#define L298N_RIGHT_BACK_PIN  PORTB1

#define L298N_RIGHT_FOR_DDR  DDRB
#define L298N_RIGHT_FOR_PORT PORTB
#define L298N_RIGHT_FOR_PIN  PORTB3

void l298n_init(void);

void l298n_left_enable(void);
void l298n_left_disable(void);
void l298n_left_toggle(void);
void l298n_left_stop(void);
void l298n_left_forward(void);
void l298n_left_backward(void);

void l298n_right_enable(void);
void l298n_right_disable(void);
void l298n_right_toggle(void);
void l298n_right_stop(void);
void l298n_right_forward(void);
void l298n_right_backward(void);

#endif
