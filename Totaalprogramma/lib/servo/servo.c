/*
Use 16-bit timer1 for controlling PWM output to control Servo motor.
OC1A (Mega D11)

Nils Bebelaar 2020
*/

#include <avr/io.h>
#include "servo.h"

//Asuming 16Mhz Board, prescalar set to 8, resulting in 2MHz clock
#define PERIOD 40000 //PWM Frequency = 2MHz / 1250 = 50 Hz

void init_servo(void)
{
    // Config pins as output
    SERVO_DDR |= (1 << SERVO_PIN);

    // Use Waveform mode 14 (datasheet p.145), clkdiv = 8, OCnA/OCnB/OCnC are set at 0, cleared at OCRnA/OCRnB/OCRnC (see below)
    TCCR1A |= 1 << WGM11 | 1 << COM1A1;
    TCCR1B |= 1 << WGM13 | 1 << WGM12 | 1 << CS11;

    //Timer resets after ICR is reached, creating 50 Hz signal
    ICR1 = PERIOD - 1;
}

//Input side (0=left, 1=right)
void moveServo(int side)
{
    if (side)
        OCR1A = 1250;
    else
        OCR1A = 8000;
}