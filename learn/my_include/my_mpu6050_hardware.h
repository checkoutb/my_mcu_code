#include "stm32f10x.h"

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


// 基于 my_mpu6050.h(软件I2C) 修改

void mpu6050_set_register(uint8_t addr, uint8_t data) {
//    i2c_start();
//    i2c_send(0xD0);
//    i2c_receive_ack();  // 没有处理
//    i2c_send(addr);
//    i2c_receive_ack();
//    i2c_send(data);
//    i2c_receive_ack();
//    i2c_stop();
    
    I2C_GenerateSTART(I2C2, ENABLE);  // 只写入，不管是否发送。 所以要等待 EV5
    while (I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);   // 循环就有可能死循环，所以需要看门狗
            // 最简单的是 while中 增加一个 计数器， 比如满10000次 就退出。
    
    I2C_Send7bitAddress(I2C2, 0xD0, I2C_Direction_Transmitter);  // 自带接收ack的功能
    while (I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS);
    
    I2C_SendData(I2C2, addr);
    while (I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING) != SUCCESS);
    
    I2C_SendData(I2C2, data);
    while (I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS);
    
    I2C_GenerateSTOP(I2C2, ENABLE);
}

uint8_t mpu6050_get_register(uint8_t addr) {
//    i2c_start();
//    i2c_send(0xD0);
//    i2c_receive_ack();
//    i2c_send(addr);
//    i2c_receive_ack();
//    
//    // 转入读的时序，必须重新指定读写位，就必须重新起始
//    i2c_start();
//    i2c_send(0xD0 | 0x01);          // 不知道为什么，估计要看芯片设计
//    i2c_receive_ack();
//    
//    // 从机发送数据
//    uint8_t res = i2c_receive();    
//    i2c_send_ack(1);   // 1 不给应答，从机不会再发送。  0的话 从机 会继续发送
//    i2c_stop();
    
    I2C_GenerateSTART(I2C2, ENABLE);
    while (I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);
    
    I2C_Send7bitAddress(I2C2, 0xD0, I2C_Direction_Transmitter);
    while (I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS);
    
    I2C_SendData(I2C2, addr);
    while (I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS);
    
    I2C_GenerateSTART(I2C2, ENABLE);
    while (I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);
    
    I2C_Send7bitAddress(I2C2, 0xD0, I2C_Direction_Receiver); // 自动 | 0x0x
    while (I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) != SUCCESS);
    
    I2C_AcknowledgeConfig(I2C2, DISABLE);  // 先设置 no ack，然后收数据。 不然 收数据的时候，自动ack，会导致从机再发一个字节。
    I2C_GenerateSTOP(I2C2, ENABLE);   // 也要提前 stop。
    
    while (I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED) != SUCCESS);
    uint8_t res = I2C_ReceiveData(I2C2);
    
    I2C_AcknowledgeConfig(I2C2, ENABLE);    // 设置会 ack
    
    return res;
}



void mpu6050_init() {
    
    // 开启iic, gpio时钟， 开漏模式， 使用结构体， 使能
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Mode = GPIO_Mode_AF_OD; // GPIO控制权要交给外设
    gpio_init.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpio_init);
    
    I2C_InitTypeDef i2c_init;
    i2c_init.I2C_Ack = I2C_Ack_Enable;
    i2c_init.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    i2c_init.I2C_ClockSpeed = 50000;
    i2c_init.I2C_DutyCycle = I2C_DutyCycle_2; // 占空比，快速状态才有用。 标准状态是1:1
    i2c_init.I2C_Mode = I2C_Mode_I2C;
    i2c_init.I2C_OwnAddress1 = 0x01;
    I2C_Init(I2C2, &i2c_init);
    
    I2C_Cmd(I2C2, ENABLE);
    
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
