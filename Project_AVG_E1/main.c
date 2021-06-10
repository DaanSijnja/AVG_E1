/*
*   Test code with the Multi function Shield
*   Code by Daan Sijnja
*   Student at HHS
*   Student code: 20177747
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "ultrasoon.h"
#include "servo.h"
#include "tone.h"
#include <util/delay.h>
int boomdetected = 0;
int A = 0;
int main(void)
{


    init_ultrasoon();
    init_servo();
    init_tone();
    playtone(NOTE_D3,500);
	sei();
    servo1_set_percentage(100);

    TCCR2A = 0;
    TCCR2B = 0b00000101;
    TIMSK2 = (1 << TOIE2);


    DDRG &= ~((1 << PG0) | (1 << PG1));
    DDRL |= (1<<PL7);

	while(1)
	{
        if((PING & (1<<PG1)) != 0)
        {
            servo1_set_percentage(-100);
            A = 1;
        }
        if((PING & (1<<PG0)) != 0)
        {
            servo1_set_percentage(90);
            A = 1;
        }

        if(((PING & (1<<PG1)) == 0) && ((PING & (1<<PG0)) == 0))
        {
            PORTL |= (1 << PL7);
            A = 0;
        }
        else
        {
            PORTL &= ~(1 << PL7);
        }

	}
}

ISR(TIMER2_OVF_vect){

    int afstand = distance(ultra_1_trigger);

    if(afstand <= 10 && boomdetected == 0 && A == 1)
    {
        boomdetected = 1;
        playtone(NOTE_A2,100);
    }
    if(afstand > 10)
    {
        boomdetected = 0;

    }


}
