/**
  ********************************************************************
  *
  * @file     stm32_sis.c
  * @author   CrystalIce
  * @version  v1.0
  * @date			2014-08-11
  * @brief		ARM 妹纸配置文件
  *
  ********************************************************************
  **/
  
#include "stm32_sis.h"
                           
static __IO uint32_t TimingDelay = 0;

void SysTick_Configuration(void)
{
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE;
  // 设置SisTick时钟 10ms 触发一次
  // SystemCoreClock / 100
  if (SysTick_Config(SystemCoreClock / 8 / 100))
    while (1);

  SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE;
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE;
  // 设置优先级
  //NVIC_SetPriority(SysTick_IRQn, 0x0);
}

void Delay(uint32_t nCount)
{
  TimingDelay = nCount;
  SysTick->CTRL |= SysTick_CTRL_ENABLE;
  while (TimingDelay != 0x0);
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE;
  SysTick->VAL = (uint32_t)0x0;
}
  
void Decrement_TimingDelay(void)
{
  //while (TimingDelay != 0x0)
    TimingDelay--;
}

