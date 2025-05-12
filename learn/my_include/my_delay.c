#include "my_delay.h"                  // Device header

void delay_us(uint32_t xus) {
    SysTick->LOAD = 72 * xus;
    SysTick->VAL = 0x0;
    SysTick->CTRL = 0x5;
    while (!(SysTick->CTRL & 0x10000));
    SysTick->CTRL = 0x4;
}

void delay_ms(uint32_t xms) {
    while (xms--)
        delay_us(1000); 
}