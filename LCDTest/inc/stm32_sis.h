/**
  *	@file			stm32_sis.h
	* @author		crystalice
	* @version	v1.0
	*	@date			2014-08-09
	* @brief		ARM妹纸头文件
	*
	*/

#ifndef __STM32_SIS_H__
#define __STM32_SIS_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f10x.h"
#include "lcd.h"
   
#define GPIO_CR_RESET                 (uint32_t)0x44444444 
#define GPIO_CR_GP_PUSHPULL           (uint32_t)0x00000000
#define GPIO_CR_GP_OPENDRAIN          (uint32_t)0x44444444 
#define GPIO_CR_AF_PUSHPULL           (uint32_t)0x88888888
#define GPIO_CR_AF_OPENDRAIN          (uint32_t)0xcccccccc 

#define GPIO_CR_OUT_2MHz              (uint32_t)0x22222222
#define GPIO_CR_OUT_10MHz             (uint32_t)0x11111111
#define GPIO_CR_OUT_50MHz             (uint32_t)0x33333333
   
#define GPIO_CR_INPUT                 (uint32_t)0x00000000  
#define GPIO_CR_FLOATINPUT            (uint32_t)0x44444444  // 浮空输入 
#define GPIO_CR_PPPDINPUT             (uint32_t)0x88888888  // 上拉/下拉模式
   
   

//#define SYSCLK_FREQ_72MHz  (uint32_t)72000000
//#define SystemCoreClock    SYSCLK_FREQ_72MHz
   
void SysTick_Configuration(void);
void Delay(uint32_t nCount);
void Decrement_TimingDelay(void);
   
#ifdef __cplusplus
 }
#endif


#endif /* __STM32_SIS_H__ */
