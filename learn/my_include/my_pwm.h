#include "stm32f10x.h"                  // Device header

void pwm_init() {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_init.GPIO_Pin = GPIO_Pin_1;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_init);
    
    TIM_InternalClockConfig(TIM2);
    TIM_TimeBaseInitTypeDef tim_init;
    tim_init.TIM_ClockDivision = TIM_CKD_DIV1;
    tim_init.TIM_CounterMode = TIM_CounterMode_Up;
    tim_init.TIM_Period = 20000 - 1;  // ARR
    tim_init.TIM_Prescaler = 72 - 1;  // PSC
    tim_init.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &tim_init);
    
    TIM_OCInitTypeDef oc_init;
    TIM_OCStructInit(&oc_init);
    oc_init.TIM_OCMode = TIM_OCMode_PWM1;
    oc_init.TIM_OCPolarity = TIM_OCPolarity_High;
    oc_init.TIM_OutputState = TIM_OutputState_Enable;
    oc_init.TIM_Pulse = 0;  // CCR 占空比
    TIM_OC2Init(TIM2, &oc_init);
    
    TIM_Cmd(TIM2, ENABLE);
}

void pwm_setcompare1(uint16_t compare) {   // 呼吸灯
    TIM_SetCompare1(TIM2, compare);
}

void pwm_setcompare2(uint16_t compare) {   // Servo
    TIM_SetCompare2(TIM2, compare);
}