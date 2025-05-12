#include "stm32f10x.h"                  // Device header
#include "my_mpu6050.h"
#include "OLED.h"
#include "my_delay.h"

// 软件模拟，使用GPIO发送电平序列
int main() {
    
    OLED_Init();
    mpu6050_init();
//    uint8_t id = mpu6050_get_register(0x75);
//    OLED_ShowNum(1, 1, id, 5); // 112..
    
//    mpu6050_set_register(0x6b, 0x00);   // 关闭只读模式  // 放到 mpu6050_init 中了

    // int a = sda_port;
    
    OLED_ShowString(1, 1, "ID:");
    OLED_ShowHexNum(1, 4, mpu6050_get_id(), 3);
    
    while (1) {
        struct MpuData res = mpu6050_get_data();
        
        OLED_ShowSignedNum(2, 1, res.accel_x, 5);
        OLED_ShowSignedNum(3, 1, res.accel_y, 5);
        OLED_ShowSignedNum(4, 1, res.accel_z, 5);
        OLED_ShowSignedNum(2, 8, res.gyro_x, 5);
        OLED_ShowSignedNum(3, 8, res.gyro_y, 5);
        OLED_ShowSignedNum(4, 8, res.gyro_z, 5);
        
        delay_ms(100);
    }
}