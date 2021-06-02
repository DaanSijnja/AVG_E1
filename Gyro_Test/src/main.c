#include <stdint.h>
#include <avr/io.h>
#include "debugPrint.h"
#include "twi.h"
#include "millis.h"
#include "VL53L0X.h"
#include "Gyrosensor.h"

//----- Functions for Clearing, Setting and Testing bits -----
#define SetBit(reg, bit) (reg |= (1 << bit))
#define ClearBit(reg, bit) (reg &= ~(1 << bit))
#define TestBit(reg, bit) ((reg & (1 << bit)) != 0)

//----- Create data storage for I2C sensors
mpu_data_t mpu_data; //MPU6050 Rotation Sensor
tof_data_t tof_data; //VL53L0X Distance Sensor

void init(void)
{
	debugInit();
	//----- Enable weak pullups on I2C lines -----
	PORTD = (1 << PD1) | (1 << PD0); //I2C on ATMega2560 is on PD1 and PD2

	i2c_init();
	millis_init();

	searchI2C();

	//----- Setup MPU6050 Rotation Sensor -----
	mpu_init(&mpu_data);		   //Initialize MPU6050
	mpu_calc_error(&mpu_data, 10); //Calculate MPU6050 error and  adjusts offsets
	mpu_set_zero(&mpu_data);	   //Set values to 0

	//----- Setup VL53L0X Time of Flight Distance Sensor -----
	initVL53L0X(1);
	writeReg(SYSTEM_INTERRUPT_CONFIG_GPIO, 0x04); //Turn on GPIO interupt, the sensor will pull GPIO to LOW when data is ready
	setMeasurementTimingBudget(33 * 1000UL);	  //Give the sensor 33ms to measure, 'Default mode' according to VL53L0X Datasheet
	startContinuous(0);							  //Start continous measurement, meaning the sensor will continue to measure distance when last data was read
}

//Read TOF sensor data and write to distance variable
void getTOFData(void)
{
	if (!TestBit(PIND, PD3))
	{
		int reading;
		reading = readReg16Bit(RESULT_RANGE_STATUS + 10); //Read data from sensor if GPIO is LOW
		writeReg(SYSTEM_INTERRUPT_CLEAR, 0x01);			  //Reset GPIO status

		if ((reading < 700) && (reading > 0))
			tof_data.distance = reading;
	}
	if (timeoutOccurred())
	{
		debug_str(" !!! Timeout !!! \n");
	}
}

int main()
{
	init();

	// Main loop
	while (1)
	{

		mpu_get_gyro(&mpu_data); //Get the rotational data
		debug_str("\nROT: ");
		debug_dec((int)mpu_data.z_angle + 360);

		getTOFData();

		debug_str("\tTOF: ");
		debug_dec(tof_data.distance);
	}
	return 0;
}