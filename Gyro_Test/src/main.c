/*
MPU6050 Gyro sensor test by Nils Bebelaar
*/

#include "Gyrosensor.h"
#include "uart.h"
#include "millisecs.h"
#include <util/delay.h>

int main(void)
{
  mpu_data_t mpu_data;            //Create MPU6050 data type
  millisecs_init();               //Initialize millisecs timer
  uart_init(115200);              //Initialize uart for debugging to serial port
  mpu_init(&mpu_data);            //Initialize MPU6050
  mpu_calc_error(&mpu_data, 500); //Calculate MPU6050 error and  adjusts offsets
  mpu_set_zero(&mpu_data);        //Set values to 0
  tw_init(TW_FREQ_400K, true);    // set I2C Frequency, enable internal pull-up

  int robotRotation = 0;

  while (1)
  {
    mpu_get_gyro(&mpu_data); //Get the rotational data

    robotRotation = (int)mpu_data.z_angle;

    printf("%i", robotRotation); //Print data to uart
    puts("");

    unsigned long bookmark = millisecs();
    while (millisecs() - bookmark < 2)
      ;
  }
}