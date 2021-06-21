/*
*       Made by Daan Sijnja
*       Student at HHS
*       Studentcode: 20177747
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "ultrasoon.h"

#define TestBit(reg, bit) ((reg & (1 << bit)) != 0)

static volatile unsigned long pulse = 0; //the variable for the pulse
static volatile int echo_pin = 0;        //logic for the ultrasoon
volatile int isTriggerd = 0;             //used in the while loop
volatile int overflows = 0;

void start_ultrasoon(int triggerpin)
{
    switch (triggerpin)
    {
    case ultra_1_trigger:
        PCMASK = 0;
        PCMASK = ultra_1_pin;
        break;

    case ultra_2_trigger:
        PCMASK = 0;
        PCMASK = ultra_2_pin;
        break;
    }

    TRIGGERPORT |= (1 << triggerpin);
    _delay_us(15);
    TRIGGERPORT &= ~(1 << triggerpin);
    _delay_us(15);
}

int ultrasoon_distance(void)
{
    return (int)(pulse / 92.8);
}

void init_ultrasoon()
{
    TRIGGERDDR |= (1 << ultra_1_trigger) | (1 << ultra_2_trigger);
    TRIGGERPORT &= ~(1 << ultra_1_trigger);
    TRIGGERPORT &= ~(1 << ultra_2_trigger);
    PCICR |= (1 << PCREG);
    PCMASK = 0;

    TIMER_B |= (1 << CS10); //used to start the internal counter of microcontroller
    TIMSK_timer = (1 << TIMSK_bit);
}

ISR(PCISR) // Interrupt service routine.
{
    if (TestBit(PORTB, ultra_1_pin))
    {
        TNCT_timer = 0;
        overflows = 0;
    }
    else
    {
        pulse = TNCT_timer + (overflows * OVERFLOW); //store the value of counter
    }
}

ISR(ISR_TIMER_OVF)
{
    overflows++;
}
