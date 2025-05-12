#ifndef __MY_I2C
#define __MY_I2C

#include "stm32f10x.h"
#include "my_delay.h"

// !!! B12, B13 !!!
// B12 - SCL   B13 - SDA

static const uint16_t sda_port = GPIO_Pin_13;      // 应该放 .c里， .h的话暴露了。
static const uint16_t scl_port = GPIO_Pin_12;

void i2c_init() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_OD;
    gpio_init.GPIO_Pin = scl_port | sda_port;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpio_init);
    
    GPIO_SetBits(GPIOB, scl_port | sda_port);
    
}

void i2c_set_scl(uint8_t value) {
    GPIO_WriteBit(GPIOB, scl_port, (BitAction) value);
    delay_us(10);
}

void i2c_set_sda(uint8_t value) {
    GPIO_WriteBit(GPIOB, sda_port, (BitAction) value);
    delay_us(10);
}

uint8_t i2c_get_scl() {
    uint8_t res = GPIO_ReadInputDataBit(GPIOB, scl_port);
    return res;
}

uint8_t i2c_get_sda() {
    uint8_t res = GPIO_ReadInputDataBit(GPIOB, sda_port);
    return res;
}

// 起始
void i2c_start() {
    i2c_set_sda(1);   // 先释放sda
    i2c_set_scl(1);
    i2c_set_sda(0);
    i2c_set_scl(0);
}

void i2c_stop() {
    i2c_set_sda(0);
    i2c_set_scl(1);
    i2c_set_sda(1);
}

void i2c_send(uint8_t byte) {
    // or 0x80 >> i
    for (uint8_t mask = 0x80; mask > 0; mask >>= 1) {
        i2c_set_sda(byte & mask);
        i2c_set_scl(1);
        i2c_set_scl(0);
    }
}

uint8_t i2c_receive() {
    i2c_set_sda(1);   // 0代表有应答，1代表没有，所以 先置没有，防止从机不存在时，之前的0 被当做 从机从机的应答
    // 还是挺怪的。如果从机不存在，这里会读到什么?
    
    uint8_t byte = 0x00;
    for (uint8_t mask = 0x80; mask > 0; mask >>= 1) {
        i2c_set_scl(1);
        if (i2c_get_sda() == 1)
            byte |= mask;
        i2c_set_scl(0);
    }
    return byte;
}


void i2c_send_ack(uint8_t ack) {
    i2c_set_sda(ack);
    i2c_set_scl(1);
    i2c_set_scl(0);
}

uint8_t i2c_receive_ack() {
    uint8_t ack = 0x00;
    i2c_set_sda(1);
    i2c_set_scl(1);
    ack = i2c_get_sda();
    i2c_set_scl(0);
    return ack;
}




#endif
