#include "Gyrosensor.h"
#include <Arduino.h>

int main(void)
{
  Serial.begin(115200);
  Serial.println("Initializing TWI_Test Project...");

  /* Initialize project configuration */
  tw_init(TW_FREQ_400K, true); // set I2C Frequency, enable internal pull-up
  mpu_init();
  mpu_data_t accel;

  Serial.println("--------------- Application Started ---------------");

  while (1)
  {
    //printf("Read gyro data.\n");
    mpu_get_gyro(&accel);
    //printf("Gyro X: %d\n", accel.x);
    //printf("Gyro Y: %d\n", accel.y);
    //printf("Gyro Z: %d\n", accel.z);
    Serial.print(accel.x);
    Serial.print(", ");
    Serial.print(accel.y);
    Serial.print(", ");
    Serial.println(accel.z);
    _delay_ms(1);
  }
}