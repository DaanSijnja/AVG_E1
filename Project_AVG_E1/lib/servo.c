/*
 * servo.c - XvR 2020
 *
 * Use 16-bit timer 1. Using interrupts in order to be
 * able to use the pins on the multi-function shield
 *
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "servo.h"

#define TIME_VALUE	(40000)
#define RESET_VALUE	(65636ul-TIME_VALUE)
#define STOP_VALUE	(TIME_VALUE*0.075)
#define RANGE		(RESET_VALUE*0.025)

ISR(TIMER_OVF_vect)
{
	TCNT1 = RESET_VALUE;

	PORT_left |= (1<<PIN_left);
	PORT_right |= (1<<PIN_right);
}

ISR(TIMER_COMPA_vect)
{
	PORT_left &= ~(1<<PIN_left);
}

ISR(TIMER_COMPB_vect)
{
	PORT_right &= ~(1<<PIN_right);
}

void init_servo(void)
{
	// Config pins as output
	DDR_left |= (1<<PIN_left);
	DDR_right |= (1<<PIN_right);

	// Use mode 0, clkdiv = 8
	TIMERA = 0;
	TIMERB = (0<<CS12) | (1<<CS11) | (0<<CS10);
	// Interrupts on OCA, OCB and OVF
	TIMERMASK = (1<<OCIE1B) | (1<<OCIE1A) | (1<<TOIE1);

	TCNT = RESET_VALUE;

	servoleft_set_percentage(0);
	servoright_set_percentage(0);

	sei();
}

void servoleft_set_percentage(float percentage)
{
	if (percentage >= -100 && percentage <= 100)
	{
		OCR1A = RESET_VALUE+STOP_VALUE+(RANGE/100.0*(percentage));
	}
}

void servoright_set_percentage(float percentage)
{
	if (percentage >= -100 && percentage <= 100)
	{
		OCR1B = RESET_VALUE+STOP_VALUE+(RANGE/100.0*(percentage));
	}
}









