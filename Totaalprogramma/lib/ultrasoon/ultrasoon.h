/*
*       Made by Daan Sijnja, adapted by Nils Bebelaar
*       Students at HHS
*       Studentcode: 20177747, 20164882
*/

#include <stdint.h>

#ifndef ULTRASOON_H_INCLUDED
#define ULTRASOON_H_INCLUDED

//settings pins **make sure you use the trigger pins on one Register like the D register
#define TRIGGERDDR DDRB
#define TRIGGERPORT PORTB
#define ultra_1_trigger PB1 //trigger pins for the ultrasoon 1 pin 52

//ultrasoon/ultrasonen
#define ultra_1_pin (1 << PCINT0) //The pin of the echo from the first ultrasoon

//Advanced settings timer
#define OVERFLOW 65535

//Returns distance in mm
uint16_t ultrasoon_distance(void);
void start_ultrasoon(void);
void init_ultrasoon(void); // inits the ultrasoon

#endif // ULTRASOON_H_INCLUDED