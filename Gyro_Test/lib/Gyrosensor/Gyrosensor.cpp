/*
MPU6050 library by Nils Bebelaar
*/

#include "Gyrosensor.h"
#include <Arduino.h>

void ERROR_CHECK(ret_code_t error_code)
{
    if (error_code != SUCCESS)
    {
        Serial.println("ERROR");
        while (1)
            ; // loop indefinitely
    }
}

void mpu_init(void)
{
    ret_code_t error_code;
    //Write 0 to PWR_MGMT_1 to turn on the sensor
    uint8_t data[2] = {PWR_MGMT_1, 0};
    error_code = tw_master_transmit(MPU6050_ADDR, data, sizeof(data), false);
    ERROR_CHECK(error_code);
}

void mpu_get_gyro_raw(mpu_data_t *mpu_data)
{
    ret_code_t error_code;
    //Need to read 6 registers, 2 for each rotation axis
    uint8_t data[6];

    //Write the first register to will be read
    data[0] = GYRO_XOUT_H;
    error_code = tw_master_transmit(MPU6050_ADDR, data, 1, true);
    ERROR_CHECK(error_code);

    //Get all the data of 6 registers
    error_code = tw_master_receive(MPU6050_ADDR, data, sizeof(data));
    ERROR_CHECK(error_code);

    //For a range of +-2g, we need to divide the raw values by 16384, according to the datasheet
    mpu_data->x = ((int16_t)(data[0] << 8 | data[1])) / 16384.0;
    mpu_data->y = ((int16_t)(data[2] << 8 | data[3])) / 16384.0;
    mpu_data->z = ((int16_t)(data[4] << 8 | data[5])) / 16384.0;
}

void mpu_get_gyro(mpu_data_t *mpu_data)
{
    mpu_get_gyro_raw(mpu_data);
    /*mpu_data->x = mpu_data->x * 9.81;
    mpu_data->y = mpu_data->y * 9.81;
    mpu_data->z = mpu_data->z * 9.81;*/
}