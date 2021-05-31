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

char getallen[10] ={
    0b00000011,
    0b10011111,
    0b00100101,
    0b00001101,
    0b10011001,
    0b01001001,
    0b01000001,
    0b00011111,
    0b00000001,
    0b00001001
};

void init (void)
{
	DDRH |= (1 << 5);
	DDRH |= (1 << 4);
	DDRG |= (1 << 5);

	PORTH &= ~(1<<4);
	PORTG &= ~(1<<5);
}


void send_data(char data)
{
    int i;
	for(i = 0; i < 8; i++)
	{
		int bit = data & (1 << i);

		if(bit)
        {
            PORTH |= (1<<5);
        }
		else
        {
            PORTH &= ~(1<<5);
        }
		PORTH |= (1<<4);
		PORTH &= ~(1<<4);
	}
}

void send_enable(char data)
{
	send_data(data);
}

void display(char data, char cijfer)
{
    PORTG &= ~(1<<5);
	send_data(data);
	send_enable(cijfer);
    PORTG |= (1<<5);
}

void displaygetal(int getal)
{
    if(getal >= 0 && getal <= 9999)
    {
        int d[4] = {(getal/1000), (getal/100) % 10,(getal/10) % 10,(getal) % 10};
        int i;
        for(i = 0; i < 4; i++)
        {
            int send = getallen[d[i]];

            if(i == 2)
            {
                send &= ~1;

            }
            display(send,(1 << 7) >> i);
        }
    }
}



int main(void)
{

    double COUNTA = 0;
    init_ultrasoon();
    init_servo();
	init();
	sei();
    servo1_set_percentage(-100);
    COUNTA = distance(ultra_1_trigger);

	while(1)
	{

        displaygetal(COUNTA*10);

	}
}
