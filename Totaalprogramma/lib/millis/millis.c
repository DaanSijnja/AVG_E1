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
  TCCR5A = 0;                                                          // turn on CTC mode
  TCCR5B = 0 | (1 << WGM52) | (0 << CS52) | (0 << CS51) | (1 << CS50); // Set CS02, CS01 and CS00 bits for 1 prescaler
  TCNT5 = 0;                                                           // initialize counter value to 0
  OCR5A = 16000;                                                       // = (16MHz / (1 prescaler * 1000Hz output))
  TIMSK5 |= (1 << OCIE5A);                                             // enable timer compare interrupt
  sei();                                                               // allow interrupts
}

//Returns the current millisecs as unsigned long
unsigned long millis(void)
{
  return currentCount;
}

//Increment currentCount variable when timer interrupt is triggered
ISR(TIMER5_COMPA_vect)
{
  currentCount++;
}