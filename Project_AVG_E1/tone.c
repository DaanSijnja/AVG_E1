/*
 * Made by Daan Sijnja
 * Student at HHS
 * Student Number: 20177747
 * With inspiration of the Tone libary for Arduino IDE
 * Made for AVR C
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "tone.h"

volatile int32_t timer_toggle_counter = 0;

void init_tone(){

    TIMERA |= (1 << 1); //ctc mode
    TIMERB = 0; //timer off

    DDRB |= (1 << PB1);
    DDR_peizo |= (1 << pin_peizo);
    PORT_peizo &= ~(1 << pin_peizo);

    sei();
}

void playtone(uint16_t freq,uint32_t duration){

    uint8_t prescaler = 0;
    int32_t toggle_count = 0;
    uint32_t OCR = 0;
    //find the right prescaler for the chosen frequenty
    OCR = (F_CPU / freq / 2) - 1;
    prescaler = 0b00000001;
    if(OCR > 255)
    {
        OCR = (F_CPU / freq / 2 / 8) - 1;
        prescaler = 0b00000010;
        if(OCR > 255)
        {
            OCR = (F_CPU / freq / 2 / 64) - 1;
            prescaler = 0b00000011;
            if(OCR > 255)
            {
                OCR = (F_CPU / freq / 2 / 256) - 1;
                prescaler = 0b00000100;
                if(OCR > 255)
                {
                    OCR = (F_CPU / freq / 2 / 1024) - 1;
                    prescaler = 0b00000101;
                }
            }
        }
    }

    //calulate the toggle count
    if(duration > 0){
        toggle_count = (2 * freq * duration) / 1000;
    }
    else
    {
        toggle_count = -1;


    }

    //set OCRA
    OCRA = OCR;
    //turn on the timer
    timer_toggle_counter = toggle_count;
    TIMERB |= prescaler;
    TIMERMASK |= (1 << 1); //enable OCRA interrupt
}


void stoptune(){
    TIMERMASK &= ~(1 << 1); //disable OCRA interrupt
}


ISR(TIMER_COMPA){

    if(timer_toggle_counter != 0){

        PORT_peizo ^= (1 << pin_peizo);

        if(timer_toggle_counter > 0)
        {
            timer_toggle_counter--;
        }
    }
    else
    {
        PORTB ^= (1 << PB1);
        TIMERMASK &= ~(1 << 1); //disable OCRA interrupt
        PORT_peizo &= ~(1 << pin_peizo); //peizo pin off
    }
}
