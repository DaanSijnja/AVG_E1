/*
 * h_bridge.h - XvR 2020
 */

#ifndef _H_BRIDGE_H_
#define _H_BRIDGE_H_

// These pins are available on the shield via the header:
//
//		Mega	Uno
// digital 5	PE3	PD5
// digital 6	PH3	PD6
// digital 9	PH6	PB1
// analog 5	PF5	PC5

// The settings below are for the Mega, modify
// in case you want to use other pins
#define PORT_RPWM PORTE
#define PIN_RPWM PE3
#define DDR_RPWM DDRE

#define PORT_LPWM PORTH
#define PIN_LPWM PH3
#define DDR_LPWM DDRH

#define ENABLEDDR DDRC
#define ENABLEPORT PORTC
#define ENABLE_1_LEFT PC0
#define ENABLE_1_RIGHT PC1
#define ENABLE_2_LEFT PC2
#define ENABLE_2_RIGHT PC3

#define FORWARDS 1
#define BACKWARDS 2
#define LEFT 3
#define RIGHT 4

void init_h_bridge(void);
void h_bridge_set_percentage(int movement, signed char percentage);

#endif /* _H_BRIDGE_H_ */