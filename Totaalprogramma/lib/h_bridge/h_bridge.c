/*
Use 8-bit timer0 for controlling 2-PWM outputs to control Left and Right motor.
OC0A (Mega D13, PB7) = Left
OC0B (Mega D4, PG5) = Right

Nils Bebelaar 2020
*/

#include <stdint.h>
#include <avr/io.h>
#include "h_bridge.h"

#define SetBit(reg, bit) (reg |= (1 << bit))
#define ClearBit(reg, bit) (reg &= ~(1 << bit))

//Asuming 16Mhz Board, prescalar set to 8, resulting in 2Mhz
#define PERIOD 250 //PWM Frequency = 2Mhz / 250 = 8Khz

void init_motors(void)
{
    // Config pins as output
    LEFT_DDR |= (1 << LEFT_PIN);
    RIGHT_DDR |= (1 << RIGHT_PIN);

    SetBit(ENABLE_DDR, ENABLE_LEFT_1);
    SetBit(ENABLE_DDR, ENABLE_LEFT_2);
    SetBit(ENABLE_DDR, ENABLE_RIGHT_1);
    SetBit(ENABLE_DDR, ENABLE_RIGHT_2);

    ClearBit(ENABLE_PORT, ENABLE_LEFT_1);
    ClearBit(ENABLE_PORT, ENABLE_LEFT_2);
    ClearBit(ENABLE_PORT, ENABLE_RIGHT_1);
    ClearBit(ENABLE_PORT, ENABLE_RIGHT_2);

    // Use Waveform mode 14 (datasheet p.145), clkdiv = 8, OCnA/OCnB/OCnC are set at 0, cleared at OCRnA/OCRnB/OCRnC (see below)
    TCCR0A |= 1 << WGM01 | 1 << WGM00 | 1 << COM0A1 | 1 << COM0B1;
    TCCR0B |= 0 << WGM02 | 1 << CS01 | 1 << CS00;

    moveMotors(0, 0);
}

void moveMotors(int8_t power_left, int8_t power_right)
{
    if (power_left > 100)
        power_left = 100;
    if (power_left < -100)
        power_left = -100;

    if (power_left == 0)
    {
        OCR0A = 0;
        ClearBit(ENABLE_PORT, ENABLE_LEFT_1);
        ClearBit(ENABLE_PORT, ENABLE_LEFT_2);
    }
    if (power_left < 0)
    {
        OCR0A = (PERIOD * power_left * (-1)) / 100;
        ClearBit(ENABLE_PORT, ENABLE_LEFT_1);
        SetBit(ENABLE_PORT, ENABLE_LEFT_2);
    }
    if (power_left > 0)
    {
        OCR0A = (PERIOD * power_left) / 100;
        ClearBit(ENABLE_PORT, ENABLE_LEFT_2);
        SetBit(ENABLE_PORT, ENABLE_LEFT_1);
    }

    if (power_right > 100)
        power_right = 100;
    if (power_right < -100)
        power_right = -100;

    if (power_right == 0)
    {
        OCR0B = 0;
        ClearBit(ENABLE_PORT, ENABLE_RIGHT_1);
        ClearBit(ENABLE_PORT, ENABLE_RIGHT_2);
    }
    if (power_right < 0)
    {
        OCR0B = (PERIOD * power_right * (-1)) / 100;
        ClearBit(ENABLE_PORT, ENABLE_RIGHT_1);
        SetBit(ENABLE_PORT, ENABLE_RIGHT_2);
    }
    if (power_right > 0)
    {
        OCR0B = (PERIOD * power_right) / 100;
        ClearBit(ENABLE_PORT, ENABLE_RIGHT_2);
        SetBit(ENABLE_PORT, ENABLE_RIGHT_1);
    }
}