/*
Millisecs by Nils Bebelaar
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "millis.h"

volatile unsigned long currentCount;

//Setup milliseconds timer; increments currentCount variable every 1 ms
void millis_init(void)
{
  TCCR3A = 0;                                                          // turn on CTC mode
  TCCR3B = 0 | (1 << WGM32) | (0 << CS32) | (0 << CS31) | (1 << CS30); // Set CS02, CS01 and CS00 bits for 1 prescaler
  TCNT3 = 0;                                                           // initialize counter value to 0
  OCR3A = 16000;                                                       // = (16MHz / (1 prescaler * 1000Hz output))
  TIMSK3 |= (1 << OCIE3A);                                             // enable timer compare interrupt
  sei();                                                               // allow interrupts
}

//Returns the current millisecs as unsigned long
unsigned long millis(void)
{
  return currentCount;
}

//Increment currentCount variable when timer interrupt is triggered
ISR(TIMER3_COMPA_vect)
{
  currentCount++;
}