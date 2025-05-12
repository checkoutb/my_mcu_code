#include "stm32f10x.h"
#include "my_delay.h"
#include "OLED.h"
#include "my_timer.h"

uint16_t cnt;

void TIM2_IRQHandler(void) {
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) {
        ++cnt;
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}

int main(void) {
    OLED_Init();
    timer_init();
    
    OLED_ShowString(1, 1, "Cnt:");
    
    
    while (1) {
        OLED_ShowNum(2, 5, cnt, 5);   // 1秒一次 。但是这里岂不是 一直在往OLED中输出。感觉应该放到上面的方法中，就是 cnt++了，才调用这个方法。
//        OLED_ShowNum(3, 5, TIM_GetCounter(TIM2), 5);   // timer_init 中 TIM_Period 是 10000，所以 0-9999。1秒1万次。
    }
}

