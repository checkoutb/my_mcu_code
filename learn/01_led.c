// #define USE_STDPERIPH_DRIVER

#include "stm32f10x.h"                  // Device header
#include "my_delay.h"


int main(void) {
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // 也可以|, | GPIOB
    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出，把LED +-极互换，依然可以闪烁， 如果是 OD 那么互换后不闪烁。
    gpio_init.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;  // 或者  | GPIO_Pin_ALL 打开所有
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_init);
//    GPIO_ResetBits(GPIOA, GPIO_Pin_0);  // SetBits  WriteBit
    
    while (1) {
        GPIO_Write(GPIOA, ~0x0001);
        delay_ms(300);
        GPIO_Write(GPIOA, ~0x0002);
        delay_ms(300);
        GPIO_Write(GPIOA, ~0x0004);
        delay_ms(300);
        GPIO_Write(GPIOA, ~0x0008);
        delay_ms(300);
        GPIO_Write(GPIOA, ~0x0010);
        delay_ms(300);
        GPIO_Write(GPIOA, ~0x0020);
        delay_ms(300);
        GPIO_Write(GPIOA, ~0x0040);
        delay_ms(300);
        GPIO_Write(GPIOA, ~0x0080);
        delay_ms(300);
    }
}