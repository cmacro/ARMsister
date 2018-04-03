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

void Delay_init(void)
{
  // 初始化SysTick 
  //   设置为 9MHz
  //SysTick_
  SysTick->CTRL &= 0xFFFFFFFB;
  fac_us = SystemCoreClock / 8 / 1000000;
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
  SysTick->LOAD = nCount * fac_ms;
  SysTick->VAL = 0x00;
  SysTick->CTRL = SysTick_CTRL_ENABLE;
  while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG));
  SysTick->CTRL = 0x00;
}

void Delay_us(uint32_t nCount)
{
  // 1微妙级延迟
  SysTick->LOAD = nCount * fac_us;
  SysTick->VAL = 0x00;
  SysTick->CTRL = SysTick_CTRL_ENABLE;
  while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG));
  SysTick->CTRL = 0x00;
}

