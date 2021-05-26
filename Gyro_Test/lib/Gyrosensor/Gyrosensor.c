/*
MPU6050 library by Nils Bebelaar
*/

#include "Gyrosensor.h"
#include "twi.h"
#include "millisecs.h"
#include "uart.h"

void ERROR_CHECK(ret_code_t error_code)
{
    if (error_code != SUCCESS)
    {
        puts("ERROR");
        while (1)
            ; // loop forever
    }
}

void mpu_init(mpu_data_t *mpu_data)
{
    //Reset angle values
    mpu_data->x_angle = 0;
    mpu_data->y_angle = 0;
    mpu_data->z_angle = 0;

    ret_code_t error_code;
    //Write 0 to PWR_MGMT_1 to turn on the sensor
    uint8_t data[2] = {PWR_MGMT_1, 0};
    error_code = tw_master_transmit(MPU6050_ADDR, data, sizeof(data), false);
    ERROR_CHECK(error_code);

    //Set current millisecs as timestamp
    mpu_data->lastTimestamp = millisecs();
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

    //For a 250deg/s range, the raw value is divided by 131.0, according to the datasheet
    mpu_data->x_acc = ((int16_t)(data[0] << 8 | data[1])) / 131.0;
    mpu_data->y_acc = ((int16_t)(data[2] << 8 | data[3])) / 131.0;
    mpu_data->z_acc = ((int16_t)(data[4] << 8 | data[5])) / 131.0;
}

//Measures the velocity and uses millisecs to determine rotated angle
void mpu_get_gyro(mpu_data_t *mpu_data)
{
    //Measure rotational velocity
    mpu_get_gyro_raw(mpu_data);

    //Correct the raw outputs with the calculated error values (See mpu_calc_error below)
    mpu_data->x_acc = mpu_data->x_acc - mpu_data->x_err;
    mpu_data->y_acc = mpu_data->y_acc - mpu_data->y_err;
    mpu_data->z_acc = mpu_data->z_acc - mpu_data->z_err;

    //Get curent milliseconds and calculate elapsed time since last measurement
    unsigned long currentMillis = millisecs();
    float elapsedSecs = (currentMillis - mpu_data->lastTimestamp) / 1000.0;

    //Only calculate new angle if time had passed
    if (elapsedSecs > 0)
    {
        mpu_data->x_angle += mpu_data->x_acc * elapsedSecs; //New angle = Old angle + (rotational velocity * time)
        mpu_data->y_angle += mpu_data->y_acc * elapsedSecs;
        mpu_data->z_angle += mpu_data->z_acc * elapsedSecs;

        //Bookmark this measurement
        mpu_data->lastTimestamp = currentMillis;
    }
}

//Calculates the error of the sensor by measuring an amount of measurements and averaging them. The code auto correct error when measured. Can be called anytime midway during a program.
void mpu_calc_error(mpu_data_t *mpu_data, int amount)
{
    for (int i = 0; i < amount; i++)
    {
        //Get raw sensor data and add it to the running total
        mpu_get_gyro_raw(mpu_data);
        mpu_data->x_err += mpu_data->x_acc;
        mpu_data->y_err += mpu_data->y_acc;
        mpu_data->z_err += mpu_data->z_acc;
    }
    //Calculate the average of the running total
    mpu_data->x_err = mpu_data->x_err / amount;
    mpu_data->y_err = mpu_data->y_err / amount;
    mpu_data->z_err = mpu_data->z_err / amount;
}