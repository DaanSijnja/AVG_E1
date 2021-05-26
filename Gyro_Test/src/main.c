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
  tw_init(TW_FREQ_400K, true);    // set I2C Frequency, enable internal pull-up

  while (1)
  {
    mpu_get_gyro(&mpu_data);              //Get the rotational data
    printf("%d,", (int)mpu_data.x_angle); //Print data to uart
    printf("%d,", (int)mpu_data.y_angle);
    printf("%d", (int)mpu_data.z_angle);
    puts("");

    unsigned long bookmark = millisecs();
    while (millisecs() - bookmark < 10);
  }
}