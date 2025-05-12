#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "my_mpu6050_hardware.h"
#include "my_delay.h"

int main() {

    OLED_Init();
    
    mpu6050_init();
    
    OLED_ShowString(1, 1, "ID:");
    OLED_ShowNum(1, 5, mpu6050_get_id(), 3);

    while (1) {
        struct MpuData data = mpu6050_get_data();
        OLED_ShowSignedNum(2, 1, data.accel_x, 5);
        OLED_ShowSignedNum(3, 1, data.accel_y, 5);
        OLED_ShowSignedNum(4, 1, data.accel_z, 5);
        OLED_ShowSignedNum(2, 7, data.gyro_x, 5);
        OLED_ShowSignedNum(3, 7, data.gyro_y, 5);
        OLED_ShowSignedNum(4, 7, data.gyro_z, 5);
        
        delay_ms(100);
    }
}