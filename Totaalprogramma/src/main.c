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
	debugInit();
	//----- Enable weak pullups on I2C lines -----
	PORTD = (1 << PD1) | (1 << PD0); //I2C on ATMega2560 is on PD1 and PD2

	i2c_init();
	millis_init();

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
}

int main()
{
	init();

	int currentState = 0;
	int treeSide = 0; //Left is 0, Right is 1

	// Main loop
	while (1)
	{
		getSensorData();

		switch (currentState)
		{
		case 0: //Startup state
			/* code */
			break;

		case 1: //Drive forward untill tree detection or front fence detection
			/* code */
			break;

		case 2: //Rotate 90 degrees (left or right, depending on which side trees were on)
			/* code */
			break;

		case 3: //Drive forward a bit
			/* code */
			break;

		case 4: //Rotate another 90 degrees (left or right, depending on which side trees were on), then flip the side, return to state 1
			/* code */
			break;

		case 10: //Emegency state
			/* code */
			break;

		default:
			break;
		}
	}
	return 0;
}