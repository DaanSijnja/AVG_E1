/*
Use 8-bit timer0 for controlling 2-PWM outputs to control Left and Right motor.
OC0A (Mega D13, PB7) = Left
OC0B (Mega D4, PG5) = Right

Nils Bebelaar 2020
*/

#include <stdint.h>

#ifndef _H_BRIDGE_H_
#define _H_BRIDGE_H_

// The settings below are for the Mega, modify
// in case you want to use other pins
#define LEFT_PORT PORTB
#define LEFT_PIN PB7
#define LEFT_DDR DDRB
#define RIGHT_PORT PORTG
#define RIGHT_PIN PG5
#define RIGHT_DDR DDRG

#define ENABLE_DDR DDRC
#define ENABLE_PORT PORTC
#define ENABLE_RIGHT_1 PC2
#define ENABLE_RIGHT_2 PC3
#define ENABLE_LEFT_1 PC4
#define ENABLE_LEFT_2 PC5

void init_motors(void);
void moveMotors(int8_t power_left, int8_t power_right);

#endif /* _H_BRIDGE_H_ */
