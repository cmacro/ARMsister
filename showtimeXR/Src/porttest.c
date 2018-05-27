#include "porttest.h"

void AM_InitTestLed(void)
{

    RCC->APB2ENR |= 1 << 2; //使能 PORTA 时钟
    RCC->APB2ENR |= 1 << 3; //使能 PORTB 时钟
    GPIOA->CRH &= 0XFFFFFFF0;
    GPIOA->CRH |= 0X00000003; //PA8 推挽输出
    GPIOA->ODR |= 1 << 8; //PA8 输出高
    GPIOB->CRL &= 0XFFFFF0FF;
    GPIOB->CRL |= 0X00000300; //PD.2 推挽输出
    GPIOB->ODR |= 1 << 2; //PD.2 输出高

    GPIOC->CRH &= 0XFFF00FFF;
    GPIOC->CRH |= 0X00038000;
    GPIOC->ODR = 1 << 11;
    GPIOC->ODR ^= 1 << 12;
}
