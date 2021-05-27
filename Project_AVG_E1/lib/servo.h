/*
 * servo.h - XvR 2020
 * Edited by Daan Sijnja 2021
 * Student at HHS
 * Studentcode 20177747
 */

#ifndef _SERVO_H_
#define _SERVO_H_


#define PORT_left	PORTE
#define PIN_left	PE3
#define DDR_left	DDRE

#define servoleft 1

#define PORT_right	PORTH
#define PIN_right	PH3
#define DDR_right	DDRH

#define servoright 1

#define PORT_3	PORTH
#define PIN_3	PH4
#define DDR_3	DDRH

#define TIMERA TCCR1A
#define TIMERB TCCR1B
#define TIMERMASK TIMSK1
#define TCNT TCNT1

#define TIMER_OVF_vect TIMER1_OVF_vect
#define TIMER_COMPA_vect TIMER1_COMPA_vect
#define TIMER_COMPB_vect TIMER1_COMPB_vect
#define TIMER_COMPC_vect TIMER1_COMPC_vect

void init_servo(void);
void servoleft_set_percentage(float percentage);
void servoright_set_percentage(float percentage);
void servo3_set_percentage(signed char percentage);

#endif /* _SERVO_H_ */
