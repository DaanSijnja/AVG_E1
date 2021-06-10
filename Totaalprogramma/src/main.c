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

//----- Create data storage for I2C sensors
mpu_data_t mpu_data; //MPU6050 Rotation Sensor
tof_data_t tof_data; //VL53L0X Distance Sensor

volatile unsigned long currentMillis, previousMillis;

void init(void)
{
	//----- Enable weak pullups on I2C lines -----
	PORTD = (1 << PD1) | (1 << PD0); //I2C on ATMega2560 is on PD1 and PD2
	SetBit(DDRA, PA6);				 //Output for tree LED

	debugInit();
	i2c_init();
	millis_init();
	init_h_bridge();
	init_ultrasoon();
	init_tone();

	//----- Setup MPU6050 Rotation Sensor -----
	mpu_init(&mpu_data);			//Initialize MPU6050
	mpu_calc_error(&mpu_data, 500); //Calculate MPU6050 error and  adjusts offsets
	mpu_set_zero(&mpu_data);		//Set values to 0

	//----- Setup VL53L0X Time of Flight Distance Sensor -----
	initVL53L0X(&tof_data, 1);
	writeReg(SYSTEM_INTERRUPT_CONFIG_GPIO, 0x04); //Turn on GPIO interupt, the sensor will pull GPIO to LOW when data is ready
	setMeasurementTimingBudget(33 * 1000UL);	  //Give the sensor 33ms to measure, 'Default mode' according to VL53L0X Datasheet
	startContinuous(0);							  //Start continous measurement, meaning the sensor will continue to measure distance when last data was read
}

void getSensorData(void)
{
	getTOFData(&tof_data, 1); //Gets TOF data and saves it to tof_data.distance
	mpu_get_gyro(&mpu_data);  //Gets gyro rotation and stores it in mpu_data.z_angle

	//Read button inputs:
	//TestBit(PORTX, PXX);
}

int main()
{
	init();

	int currentState = 0, stateBeforeEmergency = 0;
	int treeSide = 0; //Left is 0, Right is 1

	previousMillis = millis();

	// Main loop
	while (1)
	{
		getSensorData();
		currentMillis = millis();

		if (TestBit(PORTA, PA7)) //Check emergency button
		{
			if (currentState != 10)
				stateBeforeEmergency = currentState; //Save previous state
			currentState = 10;						 //State 10 is emergency state
		}

		//-----Big STM Switch -----
		switch (currentState)
		{
		case 0:			  //Startup state
			if (treeSide) //Move the tree detection sensor to the correct side
				servo_set_percentage(-100);
			else
				servo_set_percentage(90);

			if (currentMillis - previousMillis >= 500) //Give servor 500ms to move to correct position
				currentState = 1;
			else
				currentState = 0;
			break;

		case 1: //Drive forward untill tree detection or front fence detection
			h_bridge_set_percentage(FORWARDS, 45);

			if (ultrasoon_distance(ultra_1_trigger) < 200)
			{
				playtone(NOTE_A2, 500);			//Turn on note for 500ms
				SetBit(PORTA, PA6);				//Turn on LED
				previousMillis = currentMillis; //Bookmark current time
				currentState = 2;
			}

			if (tof_data.distance < 200)
			{
				mpu_set_zero(&mpu_data);
				currentState = 4;
			}

			break;

		case 2: //Tree detected, wait for 500ms
			h_bridge_set_percentage(FORWARDS, 0);

			if (currentMillis - previousMillis >= 500)
			{
				ClearBit(PORTA, PA6); //Turn off LED
				currentState = 3;
			}
			else
				currentState = 2;

			break;

		case 3: //Drive forward untill tree no longer detected, then go back to state 1
			h_bridge_set_percentage(FORWARDS, 45);

			if (ultrasoon_distance(ultra_1_trigger) > 200)
				currentState = 1;
			else
				currentState = 3;

			break;

		case 4: //Rotate 90 degrees (left or right, depending on which side trees were on)
			if (treeSide == 0)
			{
				if (mpu_data.z_angle <= -90)
				{
					h_bridge_set_percentage(FORWARDS, 0);
					previousMillis = currentMillis;
					currentState = 5;
				}
				else
					h_bridge_set_percentage(LEFT, 30);
			}
			else
			{
				if (mpu_data.z_angle >= 90)
				{
					h_bridge_set_percentage(FORWARDS, 0);
					previousMillis = currentMillis;
					currentState = 5;
				}
				else
					h_bridge_set_percentage(RIGHT, 30);
			}
			break;

		case 5: //Drive forward a bit
			h_bridge_set_percentage(FORWARDS, 30);

			if (currentMillis - previousMillis >= 500)
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
				if (mpu_data.z_angle <= -90)
				{
					h_bridge_set_percentage(FORWARDS, 0);
					treeSide = 1;
					previousMillis = currentMillis;
					currentState = 0;
				}
				else
					h_bridge_set_percentage(LEFT, 30);
			}
			else
			{
				if (mpu_data.z_angle >= 90)
				{
					h_bridge_set_percentage(FORWARDS, 0);
					treeSide = 0;
					previousMillis = currentMillis;
					currentState = 0;
				}
				else
					h_bridge_set_percentage(RIGHT, 30);
			}
			break;

		case 10:					 //Emegency state
			if (TestBit(PORTA, PA7)) //Check emergency button
				currentState = 10;

			else
				currentState = stateBeforeEmergency; //Go back to where we left off

			break;

		default:
			break;
		}
	}
	return 0;
}