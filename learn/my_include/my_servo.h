#include "stm32f10x.h"
#include "my_pwm.h"

void servo_init(void) {
    pwm_init();
}

void servo_set_angle(float angle) {
    pwm_setcompare2(angle * 2000 / 180 + 500);
}
