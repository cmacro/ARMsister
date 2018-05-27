/**
  ********************************************************************
  *
  * @file     armsis.c
  * @author   CrystalIce
  * @version  v1.0.0.1
  * @date     2014-08-11
  * @brief    ARM 妹纸配置文件
  *
  ********************************************************************
  **/

#include "armsis.h"

static __IO uint8_t   fac_us = 0;     // us 延迟
static __IO uint16_t  fac_ms = 0;     // ms 延迟

void Delay_init(uint32_t sysclock)
{
    // 初始化SysTick
    SysTick->CTRL &= 0xFFFFFFFB;
    fac_us = (sysclock >> 3) / 1000000;
    fac_ms = (uint16_t)fac_us * 1000;
}

void Delay(uint32_t nCount)
{
    // 10 毫秒延迟
    Delay_ms(nCount * 10);
}

void Delay_ms(uint16_t nCount)
{
    // 1毫秒级延迟
    SysTick->LOAD = (uint32_t)nCount * fac_ms;
    SysTick->VAL = 0x00;
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;
    while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
    SysTick->CTRL = 0x00;
}

void Delay_us(uint32_t nCount)
{
    // 1微妙级延迟
    SysTick->LOAD = (uint32_t)nCount * fac_us;
    SysTick->VAL = 0x00;
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;
    while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
    SysTick->CTRL = 0x00;
}

void _NVIC_PriorityGroupConfig(uint8_t group)
{
    uint32_t temp, temp1;
    temp1 = (~group) & 0x07;//取后三位
    temp1 <<= 8;
    temp = SCB->AIRCR;      //读取先前的设置
    temp &= 0X0000F8FF;     //清空先前分组
    temp |= 0X05FA0000;     //写入钥匙
    temp |= temp1;
    SCB->AIRCR = temp;          //设置分组
}

void _NVIC_Init(uint8_t preemption, uint8_t sub, uint8_t channel,
                uint8_t group)
{
    uint32_t temp;
    _NVIC_PriorityGroupConfig(group);    //设置分组
    temp = preemption << (4 - group);
    temp |= sub & (0x0f >> group);
    temp &= 0xf;                        //取低四位
    NVIC->ISER[channel / 32] |= (1 << channel % 32);
    //使能中断位(要清除的话,相反操作就 OK)
    NVIC->IP[channel] |= temp << 4;//设置响应优先级和抢断优先级
}


void NVIC_Config(void) {
    _NVIC_Init(0, 0, RTC_IRQn, 2);
}

void RCC_init(void)
{
    // 使用外部8M晶振，启用PPL，系统时钟为48MHz，总线时钟12HMz
    RCC->CFGR = RCC_CFGR_SW_PLL |       // 启用PPL
                RCC_CFGR_PLLSRC |       // 使用PPL作为时钟源
                RCC_CFGR_PLLMULL6 |     // 6倍频，系统时钟 48Mhz
                RCC_CFGR_HPRE_DIV4 |    // 预分频，总线时钟 12MHz
                RCC_CFGR_PPRE1_DIV1 |   // 低速总线 12MHz
                RCC_CFGR_PPRE2_DIV1 |   // 高速总线 12MHz
                RCC_CFGR_USBPRE;        // USB直接使用PPL倍频时钟 48MHz

    // 开启PPL和外部时钟
    RCC->CR =   RCC_CR_PLLON |
                RCC_CR_HSEON;

    // 闪存访问延迟，48MHz ~ 72MHz = 010。 复位值：0x30
    // 01 Flash访问周期 (24Mhz ~ 48MHz] 范围
    // AHB时钟分频 != 1 时必须开启
    FLASH->ACR = FLASH_ACR_LATENCY_0 |
                 FLASH_ACR_PRFTBE |
                 FLASH_ACR_PRFTBS;


    // 确定外部高速晶振起效
    while (!(RCC->CR & RCC_CR_HSERDY));
    // 确定PLL设置起效
    while (!(RCC->CR & RCC_CR_PLLRDY));
    // 确定PLL为系统时钟源
    while (!(RCC->CFGR & RCC_CFGR_SWS_PLL));
}



