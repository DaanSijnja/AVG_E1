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

volatile int16_t distance;
volatile int8_t overflow;

void start_ultrasoon()
{
    TCCR5B = 0;
    TIMSK5 = 0;

    SetBit(TRIGGERPORT, ultra_1_trigger);
    _delay_us(15);
    ClearBit(TRIGGERPORT, ultra_1_trigger);

    TCNT5 = 0;
    TCCR5B |= (1 << CS51) | (1 << ICNC5) | (0 << ICES5);
    TIMSK5 = 0 | (1 << TOIE5);
    overflow = 0;
}

uint16_t ultrasoon_distance(void)
{
    if (overflow == 0)
        distance = (ICR5L) | (ICR5H << 8);
    return (distance - 60000);
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

ISR(TIMER5_OVF_vect)
{
    overflow = 1;
}