#include "stm32f10x.h"
#include "my_i2c.h"

#define	MPU6050_SMPLRT_DIV		0x19
#define	MPU6050_CONFIG			0x1A
#define	MPU6050_GYRO_CONFIG		0x1B
#define	MPU6050_ACCEL_CONFIG	0x1C

#define	MPU6050_ACCEL_XOUT_H	0x3B
#define	MPU6050_ACCEL_XOUT_L	0x3C
#define	MPU6050_ACCEL_YOUT_H	0x3D
#define	MPU6050_ACCEL_YOUT_L	0x3E
#define	MPU6050_ACCEL_ZOUT_H	0x3F
#define	MPU6050_ACCEL_ZOUT_L	0x40
#define	MPU6050_TEMP_OUT_H		0x41
#define	MPU6050_TEMP_OUT_L		0x42
#define	MPU6050_GYRO_XOUT_H		0x43
#define	MPU6050_GYRO_XOUT_L		0x44
#define	MPU6050_GYRO_YOUT_H		0x45
#define	MPU6050_GYRO_YOUT_L		0x46
#define	MPU6050_GYRO_ZOUT_H		0x47
#define	MPU6050_GYRO_ZOUT_L		0x48

#define	MPU6050_PWR_MGMT_1		0x6B
#define	MPU6050_PWR_MGMT_2		0x6C
#define	MPU6050_WHO_AM_I		0x75

void mpu6050_set_register(uint8_t addr, uint8_t data) {
    i2c_start();
    i2c_send(0xD0);
    i2c_receive_ack();  // 没有处理
    i2c_send(addr);
    i2c_receive_ack();
    i2c_send(data);
    i2c_receive_ack();
    i2c_stop();
}

uint8_t mpu6050_get_register(uint8_t addr) {
    i2c_start();
    i2c_send(0xD0);
    i2c_receive_ack();
    i2c_send(addr);
    i2c_receive_ack();
    
    // 转入读的时序，必须重新指定读写位，就必须重新起始
    i2c_start();
    i2c_send(0xD0 | 0x01);          // 不知道为什么，估计要看芯片设计
    i2c_receive_ack();
    
    // 从机发送数据
    uint8_t res = i2c_receive();    
    i2c_send_ack(1);   // 1 不给应答，从机不会再发送。  0的话 从机 会继续发送
    i2c_stop();
    
    return res;
}



void mpu6050_init() {
    i2c_init();
    mpu6050_set_register(MPU6050_PWR_MGMT_1, 0x01);
    mpu6050_set_register(MPU6050_PWR_MGMT_2, 0x00);
    mpu6050_set_register(MPU6050_SMPLRT_DIV, 0x09);  // 十分频
    mpu6050_set_register(MPU6050_CONFIG, 0x06);
    mpu6050_set_register(MPU6050_GYRO_CONFIG, 0x18);  // 最大量程
    mpu6050_set_register(MPU6050_ACCEL_CONFIG, 0x18);
    
}

uint8_t mpu6050_get_id() {
    return mpu6050_get_register(MPU6050_WHO_AM_I);
}

struct MpuData {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
};

struct MpuData mpu6050_get_data() {
    struct MpuData res;
    uint8_t dh, dl;
    
    // 可以使用i2c 读取连续字节，而不是 12次。
    
    dh = mpu6050_get_register(MPU6050_ACCEL_XOUT_H);
    dl = mpu6050_get_register(MPU6050_ACCEL_XOUT_L);
    res.accel_x = (dh << 8) | dl;
    
    dh = mpu6050_get_register(MPU6050_ACCEL_YOUT_H);
    dl = mpu6050_get_register(MPU6050_ACCEL_YOUT_L);
    res.accel_y = (dh << 8) | dl;
    
    dh = mpu6050_get_register(MPU6050_ACCEL_ZOUT_H);
    dl = mpu6050_get_register(MPU6050_ACCEL_ZOUT_L);
    res.accel_z = (dh << 8) | dl;
    
    dh = mpu6050_get_register(MPU6050_GYRO_XOUT_H);
    dl = mpu6050_get_register(MPU6050_GYRO_XOUT_L);
    res.gyro_x = (dh << 8) | dl;
        
    dh = mpu6050_get_register(MPU6050_GYRO_YOUT_H);
    dl = mpu6050_get_register(MPU6050_GYRO_YOUT_L);
    res.gyro_y = (dh << 8) | dl;
    
    dh = mpu6050_get_register(MPU6050_GYRO_ZOUT_H);
    dl = mpu6050_get_register(MPU6050_GYRO_ZOUT_L);
    res.gyro_z = (dh << 8) | dl;
    
    return res;
}
