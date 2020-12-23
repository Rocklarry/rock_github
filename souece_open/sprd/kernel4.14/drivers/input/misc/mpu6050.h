/*
 * Definitions for mma8452 compass chip.
 */
#ifndef MPU6050_H
#define MPU6050_H

#include <linux/mutex.h>
#include <linux/i2c.h>
#include <linux/input-polldev.h>
#include <linux/mutex.h>

#define MPU6050_XG_OFFSET_H		0x13
#define MPU6050_XG_OFFSET_L		0x14
#define MPU6050_YG_OFFSET_H		0x15
#define MPU6050_YG_OFFSET_L		0x16
#define MPU6050_ZG_OFFSET_H		0x17
#define MPU6050_ZG_OFFSET_L		0x18

#define MPU6050_SMPLRT_DIV			0x19

#define MPU6050_CONFIG				0x1A
#define MPU6050_GYRO_CONFIG		0x1B
#define MPU6050_ACCEL_CONFIG		0x1C
#define MPU6050_ACCEL_CONFIG2 	0x1D

#define MPU6050_LP_ACCEL_ODR 		0x1E
#define MPU6050_WOM_THR			0x1F

#define MPU6050_FIFO_EN			0x23

#define MPU6050_INT_PIN_CFG		0x37
#define MPU6050_INT_ENABLE  		0x38
#define MPU6050_INT_STATUS  		0x3A

#define MPU6050_ACCEL_XOUT_H 		0x3B
#define MPU6050_ACCEL_XOUT_L		0x3C
#define MPU6050_ACCEL_YOUT_H 		0x3D
#define MPU6050_ACCEL_YOUT_L		0x3E
#define MPU6050_ACCEL_ZOUT_H 		0x3F
#define MPU6050_ACCEL_ZOUT_L		0x40
#define MPU6050_TEMP_OUT_H		0x41
#define MPU6050_TEMP_OUT_L		0x42
#define MPU6050_GYRO_XOUT_H		0x43
#define MPU6050_GYRO_XOUT_L 		0x44
#define MPU6050_GYRO_YOUT_H		0x45
#define MPU6050_GYRO_YOUT_L		0x46
#define MPU6050_GYRO_ZOUT_H		0x47
#define MPU6050_GYRO_ZOUT_L 		0x48

#define MPU6050_SIGNAL_PATH_RESET	0x68
#define MPU6050_ACCEL_INTEL_CTRL  	0x69
#define MPU6050_USER_CTRL		0x6A
#define MPU6050_PWR_MGMT_1 		0x6B
#define MPU6050_PWR_MGMT_2		0x6C
#define MPU6050_FIFO_COUNTH 		0x72
#define MPU6050_FIFO_COUNTL		0x73
#define MPU6050_FIFO_R_W  		0x74

#define MPU6050_WHO_AM_I 		0x75

#define MPU6050_PWRM1_SLEEP		0x40
#define MPU6050_PWRM1_GYRO_STANDBY	0x10
#define MPU6050_PWRM2_ACCEL_DISABLE	0x38
#define MPU6050_PWRM2_GYRO_DISABLE	0x07

struct mpu6050_device
{
	struct i2c_client *client;
	struct input_polled_dev *input_dev;
	struct mutex mutex;
	short  accel_status;
	short  gyro_status;
	unsigned int accel_poll;
	unsigned int gyro_poll;
	unsigned int poll_time;
	short accel_data[3];
	short gyro_data[3];
	short accel_offset[3];
	short gyro_offset[3];
};


#endif

