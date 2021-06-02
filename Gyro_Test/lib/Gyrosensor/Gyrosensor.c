/*
MPU6050 library by Nils Bebelaar

Currently only reads Z-Axis, others are commented out
*/

#include "Gyrosensor.h"
#include "twi.h"
#include "millis.h"
#include "debugPrint.h"

void mpu_init(mpu_data_t *mpu_data)
{
    //Reset angle values
    //mpu_data->x_angle = 0;
    //mpu_data->y_angle = 0;
    mpu_data->z_angle = 0;

    //Write 0 to PWR_MGMT_1 to turn on the sensor
    if ((i2c_start((MPU6050_ADDR << 1) | I2C_WRITE)) == 0)
    {
        i2c_write(PWR_MGMT_1);
        i2c_write(0);
        i2c_stop();
    }
    else
        debug_str("\nCannot communicate with MPU6050\n");

    //Set current millisecs as timestamp
    mpu_data->lastTimestamp = millis();
}

void mpu_get_gyro_raw(mpu_data_t *mpu_data)
{
    //Need to read 6 registers, 2 for each rotation axis
    uint8_t data[2];

    if ((i2c_start((MPU6050_ADDR << 1) | I2C_WRITE)) == 0)
    {
        i2c_write(GYRO_ZOUT_H);
        i2c_rep_start((MPU6050_ADDR << 1) | I2C_READ);
        data[0] = i2c_readAck();
        //data[1] = i2c_readAck();
        //data[2] = i2c_readAck();
        //data[3] = i2c_readAck();
        //data[4] = i2c_readAck();
        data[1] = i2c_readNak();
        i2c_stop();
    }

    //For a 250deg/s range, the raw value is divided by 131.0, according to the datasheet
    //mpu_data->x_acc = ((int16_t)(data[0] << 8 | data[1])) / 131.0;
    //mpu_data->y_acc = ((int16_t)(data[2] << 8 | data[3])) / 131.0;
    mpu_data->z_acc = ((int16_t)(data[0] << 8 | data[1])) / 131.0;
}

//Measures the velocity and uses millisecs to determine rotated angle
void mpu_get_gyro(mpu_data_t *mpu_data)
{
    //Measure rotational velocity
    mpu_get_gyro_raw(mpu_data);

    //Correct the raw outputs with the calculated error values (See mpu_calc_error below)
    //mpu_data->x_acc = mpu_data->x_acc - mpu_data->x_err;
    //mpu_data->y_acc = mpu_data->y_acc - mpu_data->y_err;
    mpu_data->z_acc = mpu_data->z_acc - mpu_data->z_err;

    //Get curent milliseconds and calculate elapsed time since last measurement
    unsigned long currentMillis = millis();
    float elapsedSecs = (currentMillis - mpu_data->lastTimestamp) / 1000.0;

    //Only calculate new angle if time has passed
    if (elapsedSecs > 0)
    {
        //mpu_data->x_angle += mpu_data->x_acc * elapsedSecs; //New angle = Old angle + (rotational velocity * time)
        //mpu_data->y_angle += mpu_data->y_acc * elapsedSecs;
        mpu_data->z_angle += mpu_data->z_acc * elapsedSecs;

        //Bookmark time of this measurement
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
        //mpu_data->x_err += mpu_data->x_acc;
        //mpu_data->y_err += mpu_data->y_acc;
        mpu_data->z_err += mpu_data->z_acc;
    }
    //Calculate the average of the running total
    //mpu_data->x_err = mpu_data->x_err / amount;
    //mpu_data->y_err = mpu_data->y_err / amount;
    mpu_data->z_err = mpu_data->z_err / amount;
}

//Reset angle values to 0
void mpu_set_zero(mpu_data_t *mpu_data)
{
    //mpu_data->x_angle = 0;
    //mpu_data->y_angle = 0;
    mpu_data->z_angle = 0;
}