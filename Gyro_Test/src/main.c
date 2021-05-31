/*
 * vl53l0xExample.c
 *
 *  Created on: July 3, 2017
 *      Author: michael
 *
 *  Demonstrate the VL53I0X distance sensor.
 * 
 */
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "debugPrint.h"
#include "twi.h"
#include "millis.h"
#include "VL53L0X.h"

//--------------------------------------------------
// Global Defines
//--------------------------------------------------
// GPIO
#define PIN_I2C_SDA PD1
#define PIN_I2C_SCL PD0


// Pseudo functions for Clearing, Setting and Testing bits
#define SBI(reg, bit) (reg |= (1 << bit))
#define CBI(reg, bit) (reg &= ~(1 << bit))
#define IBI(reg, bit) ((reg & (1 << bit)) != 0)

void init(void)
{
	debugInit();
	//--------------------------------------------------
	// GPIOs
	//--------------------------------------------------
	CBI(UCSR0B, RXEN0);		   // Disable UART RX
	// Enable weak pullups on I2C lines
	PORTD = (1 << PIN_I2C_SCL) | (1 << PIN_I2C_SDA);
	//--------------------------------------------------
	// Init the other modules
	//--------------------------------------------------
	i2c_init();
	millis_init();
	sei();
}

int main()
{
	tof_data_t tof_data;
	init();

	initVL53L0X(1);
	// lower the return signal rate limit (default is 0.25 MCPS)
	// setSignalRateLimit(0.1);
	// increase laser pulse periods (defaults are 14 and 10 PCLKs)
	// setVcselPulsePeriod(VcselPeriodPreRange, 18);
	// setVcselPulsePeriod(VcselPeriodFinalRange, 14);
	setMeasurementTimingBudget(500 * 1000UL); // integrate over 500 ms per measurement

	// Main loop
	while (1)
	{
		readRangeSingleMillimeters(&tof_data); // blocks until measurement is finished
		debug_str("\nTOF: ");
		debug_dec(tof_data.rawDistance);
		if (timeoutOccurred())
		{
			debug_str(" !!! Timeout !!! \n");
		}
	}
	return 0;
}