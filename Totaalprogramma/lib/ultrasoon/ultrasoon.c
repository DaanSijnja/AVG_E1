/*
*       Made by Daan Sijnja, adapted by Nils Bebelaar
*       Students at HHS
*       Studentcode: 20177747, 20164882
*/

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "ultrasoon.h"

#include "debugPrint.h"

//----- Functions for Clearing, Setting and Testing bits -----
#define SetBit(reg, bit) (reg |= (1 << bit))
#define ClearBit(reg, bit) (reg &= ~(1 << bit))
#define TestBit(reg, bit) ((reg & (1 << bit)) != 0)

volatile uint16_t distance;

void start_ultrasoon()
{
    TCCR5B = 0;
    TIMSK5 = 0;

    SetBit(TRIGGERPORT, ultra_1_trigger);
    _delay_us(15);
    ClearBit(TRIGGERPORT, ultra_1_trigger);

    TCNT5 = 0;
    TCCR5B |= (1 << CS51) | (1 << ICNC5) | (0 << ICES5);
}

uint16_t ultrasoon_distance(void)
{
    distance = (ICR5L) | (ICR5H << 8);
    distance = distance - 7300; //Subtract time between Trigger and first Echo edge
    return (uint16_t)((distance * 10) / 92.8);
}

void init_ultrasoon()
{
    SetBit(TRIGGERDDR, ultra_1_trigger);
    ClearBit(TRIGGERPORT, ultra_1_trigger);

    TCCR5A = 0;
    TCCR5B = 0;
    TIMSK5 = 0;

    sei();
}
