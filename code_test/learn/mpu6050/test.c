#include<stdio.h>
#include <sys/ioctl.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

#include "mpu6050_common.h"
#define MPU6050_MAGIC 'K'


int main(int argc, char * const argv[])
{
    int fd = open(argv[1],O_RDWR);
    if(-1== fd){
        perror("open");
        return -1;
    }
    union mpu6050_data data = {{0}};
    while(1){
        ioctl(fd,GET_ACCEL,&data);
        printf("acc:x %d, y:%d, z:%d\n",data.accel.x,data.accel.y,data.accel.z);
        ioctl(fd,GET_GYRO,&data);
        printf("gyro:x %d, y:%d, z:%d\n",data.gyro.x,data.gyro.y,data.gyro.z);
        ioctl(fd,GET_TEMP,&data);
        printf("temp: %d\n",data.temp);
        sleep(1);
    }
    return 0;
}