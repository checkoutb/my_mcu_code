#include "stm32f10x.h"                  // Device header
#include "my_servo.h"
#include "my_delay.h"
#include "OLED.h"
#include "Key.h"

int main() {
    
    OLED_Init();
    servo_init();
    Key_Init();
    
    OLED_ShowString(1, 1, "Angle:");

    int angle = 0;
    while (1) {
        if (Key_GetNum() == 1) {
            angle += 30;
            if (angle > 180)
                angle = 0;
            servo_set_angle(angle);
            OLED_ShowNum(2, 7, angle, 3);
        }
    }
    
}