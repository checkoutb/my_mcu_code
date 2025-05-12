#include "stm32f10x.h"                  // Device header

void timer_init(void) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    TIM_InternalClockConfig(TIM2);
    
    TIM_TimeBaseInitTypeDef tim_init;
    tim_init.TIM_ClockDivision = TIM_CKD_DIV1;
    tim_init.TIM_CounterMode = TIM_CounterMode_Up;
    tim_init.TIM_Period = 10000 - 1;
    tim_init.TIM_Prescaler = 7200 - 1;
    tim_init.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &tim_init);
    
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);  // 
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    NVIC_InitTypeDef nvic_init;
    nvic_init.NVIC_IRQChannel = TIM2_IRQn;
    nvic_init.NVIC_IRQChannelCmd = ENABLE;
    nvic_init.NVIC_IRQChannelPreemptionPriority = 2;
    nvic_init.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&nvic_init);
    
    TIM_Cmd(TIM2, ENABLE);
}

/*
void TIM2_IRQHandler(void) {
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
*/
