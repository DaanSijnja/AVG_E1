#include "Gyrosensor.h"
#include "uart.h"

int main(void)
{
  uart_init(115200);
  puts("Initializing TWI_Test Project...");

  /* Initialize project configuration */
  tw_init(TW_FREQ_400K, true); // set I2C Frequency, enable internal pull-up
  mpu_init();
  mpu_data_t accel;

  puts("--------------- Application Started ---------------");

  while (1)
  {
    //printf("Read gyro data.\n");
    mpu_get_gyro(&accel);
    //printf("Gyro X: %d\n", accel.x);
    //printf("Gyro Y: %d\n", accel.y);
    //printf("Gyro Z: %d\n", accel.z);
    printf("%d, ", (int)accel.x);
    printf("%d, ", (int)accel.y);
    printf("%d\n", (int)accel.z);
    _delay_ms(1);
  }
}