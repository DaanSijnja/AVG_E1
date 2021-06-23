//--------------------------------------------------------//
// De Haagste Hogeschool, Project AGV 2020-2021, Groep E2 //
//--------------------------------------------------------//

#include <stdint.h>
#include <avr/io.h>
#include "debugPrint.h"
#include "twi.h"
#include "millis.h"
#include "VL53L0X.h"
#include "Gyrosensor.h"
#include "ultrasoon.h"
#include "h_bridge.h"
#include "servo.h"
#include "tone.h"

//----- Functions for Clearing, Setting and Testing bits -----
#define SetBit(reg, bit) (reg |= (1 << bit))
#define ClearBit(reg, bit) (reg &= ~(1 << bit))
#define TestBit(reg, bit) ((reg & (1 << bit)) != 0)

//----- Defines for pins -----
#define IR_LEFT PL6	 //pin 43
#define IR_RIGHT PL4 //pin 45

#define switch_right PG5 //pin 4
#define switch_left PE3	 //pin 5

#define led_green PA6 //pin 28
#define led_red PA7	  //pin 29

//----- Create data storage for I2C sensors
mpu_data_t mpu_data; //MPU6050 Rotation Sensor
tof_data_t tof_data; //VL53L0X Distance Sensor

volatile unsigned long currentMillis, previousMillis, previousMillisUltrasoon, previousMillisTOF;

void init(void)
{
	SetBit(DDRA, PA6);
	SetBit(DDRA, PA7); //Output for tree LED

	ClearBit(DDRL, IR_LEFT);
	ClearBit(DDRL, IR_RIGHT);

	debugInit();
	i2c_init();
	millis_init();
	init_motors();
	init_servo();
	init_ultrasoon();
	init_tone();

	searchI2C();

	//----- Setup MPU6050 Rotation Sensor -----
	mpu_init(&mpu_data);			//Initialize MPU6050
	mpu_calc_error(&mpu_data, 500); //Calculate MPU6050 error and  adjusts offsets
	mpu_set_zero(&mpu_data);		//Set values to 0

	//----- Setup VL53L0X Time of Flight Distance Sensor -----
	initVL53L0X(&tof_data, 1);
	writeReg(SYSTEM_INTERRUPT_CONFIG_GPIO, 0x04); //Turn on GPIO interupt, the sensor will pull GPIO to LOW when data is ready
	setMeasurementTimingBudget(33 * 1000UL);	  //Give the sensor 33ms to measure, 'Default mode' according to VL53L0X Datasheet
	startContinuous(0);							  //Start continous measurement, meaning the sensor will continue to measure distance when last data was read

	tof_data.distance = 1000;
}

void getSensorData(void)
{
	getTOFData(&tof_data, 1); //Gets TOF data and saves it to tof_data.distance
	mpu_get_gyro(&mpu_data);  //Gets gyro rotation and stores it in mpu_data.z_angle

	if (currentMillis - previousMillisUltrasoon > 100)
	{
		start_ultrasoon();
		previousMillisUltrasoon = currentMillis;
	}

	debug_str("\nROT: ");
	debug_dec((int)mpu_data.z_angle + 360);
	debug_str("\t  TOF: ");
	debug_dec(tof_data.distance);

	debug_str("\tSONIC: ");
	debug_dec(ultrasoon_distance());

	debug_str("\t IR Left: ");
	debug_dec(!TestBit(PINL, IR_LEFT));

	debug_str("\t IR Right: ");
	debug_dec(!TestBit(PINL, IR_RIGHT));

	//Read button inputs:
	//TestBit(PORTX, PXX);
}

int main()
{
	init();

	int currentState = 0;
	int treeSide = 0; //Left is 0, Right is 1
	int pathsDone = 0;

	previousMillis = millis();
	previousMillisUltrasoon = millis();
	previousMillisTOF = millis();

	moveServo(0);

	// Main loop
	while (1)
	{
		currentMillis = millis();
		getSensorData();
		debug_str("\tCase: ");
		debug_dec(currentState);

		if (pathsDone >= 3)
		{
			playtone(NOTE_A2, 500);
			treeSide = 0;
			pathsDone = 0;
		}

		//-----Big STM Switch -----
		switch (currentState)
		{
		case 0:
			//Startup state
			moveServo(treeSide); //Move the tree detection sensor to the correct side

			if (currentMillis - previousMillis >= 500) //Give servor 500ms to move to correct position
			{
				currentState = 1;
				previousMillis = currentMillis;
				previousMillisTOF = currentMillis;
			}
			else
				currentState = 0;

			break;

		case 1: //Drive forward untill tree detection or front fence detection
			if (TestBit(PINL, IR_LEFT) == TestBit(PINL, IR_RIGHT))
				moveMotors(60, 60);
			else
			{
				if (!TestBit(PINL, IR_RIGHT))
					moveMotors(40, 75);
				if (!TestBit(PINL, IR_LEFT))
					moveMotors(75, 40);
			}

			if (ultrasoon_distance() < 8000)
			{
				SetBit(PORTA, PA7);
				playtone(NOTE_A6, 500);			//Turn on note for 500ms
				SetBit(PORTA, led_red);			//Turn on LED
				previousMillis = currentMillis; //Bookmark current time
				currentState = 2;
			}

			if ((tof_data.distance < 200) && (currentMillis - previousMillisTOF >= 2000))
			{
				mpu_set_zero(&mpu_data);
				currentState = 4;
			}

			break;

		case 2: //Tree detected, wait for 500ms
			moveMotors(0, 0);

			if (currentMillis - previousMillis >= 500)
			{
				ClearBit(PORTA, led_red); //Turn off LED
				previousMillis = currentMillis;
				currentState = 3;
			}
			else
				currentState = 2;

			break;

		case 3: //Drive forward untill tree no longer detected, then go back to state 1
			if (TestBit(PINL, IR_LEFT) == TestBit(PINL, IR_RIGHT))
				moveMotors(60, 60);
			else
			{
				if (!TestBit(PINL, IR_RIGHT))
					moveMotors(40, 75);
				if (!TestBit(PINL, IR_LEFT))
					moveMotors(75, 40);
			}

			if ((ultrasoon_distance() > 8000) && (currentMillis - previousMillis >= 350))
			{
				previousMillis = currentMillis;
				currentState = 1;
			}
			else
				currentState = 3;

			break;

		case 4: //Rotate 90 degrees (left or right, depending on which side trees were on)
			if (treeSide == 0)
			{
				if (mpu_data.z_angle <= -80)
				{
					moveMotors(0, 0);
					previousMillis = currentMillis;
					currentState = 5;
				}
				else
					moveMotors(-60, 60);
			}
			else
			{
				if (mpu_data.z_angle >= 80)
				{
					moveMotors(0, 0);
					previousMillis = currentMillis;
					currentState = 5;
				}
				else
					moveMotors(70, -50);
			}
			break;

		case 5: //Drive forward a bit
			moveMotors(60, 55);

			if (currentMillis - previousMillis >= 600)
			{
				mpu_set_zero(&mpu_data);
				currentState = 6;
			}
			else
				currentState = 5;
			break;

		case 6: //Rotate another 90 degrees (left or right, depending on which side trees were on), then flip the side, return to state 1
			if (treeSide == 0)
			{
				if (mpu_data.z_angle <= -80)
				{
					moveMotors(0, 0);
					treeSide = 1;
					previousMillis = currentMillis;
					currentState = 0;
					pathsDone++;
				}
				else
					moveMotors(-60, 60);
			}
			else
			{
				if (mpu_data.z_angle >= 80)
				{
					moveMotors(0, 0);
					treeSide = 0;
					previousMillis = currentMillis;
					currentState = 0;
					pathsDone++;
				}
				else
					moveMotors(70, -50);
			}
			break;

		default:
			break;
		}
	}
	return 0;
}