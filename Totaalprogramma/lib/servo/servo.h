/*
Use 16-bit timer1 for controlling PWM output to control Servo motor.
OC1A (Mega D11)

Nils Bebelaar 2020
*/

#ifndef _SERVO_H_
#define _SERVO_H_

// The settings below are for the Mega, modify
// in case you want to use other pins
#define SERVO_PORT PORTB
#define SERVO_PIN PB5
#define SERVO_DDR DDRB

void init_servo(void);
void moveServo(int angle);

#endif /* _SERVO_H_ */
