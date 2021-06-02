/*
MPU6050 library by Nils Bebelaar
*/

#include "twi.h"
#include "millis.h"

#define MPU6050_ADDR 0x68

/* MPU6050 register address */
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define PWR_MGMT_1 0x6B
#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48

typedef struct
{
    //float x_acc;
    //float y_acc;
    float z_acc;
    //float x_err;
    //float y_err;
    float z_err;
    //float x_angle;
    //float y_angle;
    float z_angle;
    unsigned long lastTimestamp;
} mpu_data_t;

void mpu_init(mpu_data_t *mpu_data);
void mpu_get_gyro_raw(mpu_data_t *mpu_data);
void mpu_get_gyro(mpu_data_t *mpu_data);
void mpu_calc_error(mpu_data_t *mpu_data, int amount);
void mpu_set_zero(mpu_data_t *mpu_data);