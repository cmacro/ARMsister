/**
  *	@file       armsis.h
  * @author     crystalice
  * @version    v1.0
  * @date       2014-08-09
  * @brief      ARM sister header file
  *
  */

#ifndef __ARMSIS_H__
#define __ARMSIS_H__

#ifdef __cplusplus
 extern "C" {
#endif
   

#include "stm32f1xx.h"
#include "serial.h"
   
#define GPIO_CR_RESET                 (uint32_t)0x44444444 
#define GPIO_CR_MODE_INPUT            (uint32_t)0x00000000
#define GPIO_CR_MODE_2MHz             (uint32_t)0x22222222
#define GPIO_CR_MODE_10MHz            (uint32_t)0x11111111
#define GPIO_CR_MODE_50MHz            (uint32_t)0x33333333

#define GPIO_CR_GP_PUSHPULL           (uint32_t)0x00000000
#define GPIO_CR_GP_OPENDRAIN          (uint32_t)0x44444444 
#define GPIO_CR_OUT_PP2MHz            (GPIO_CR_MODE_2MHz | GPIO_CR_GP_PUSHPULL)
#define GPIO_CR_OUT_PP50MHz           (GPIO_CR_MODE_50MHz | GPIO_CR_GP_PUSHPULL)

   
#define GPIO_CR_AFO_PUSHPULL           (uint32_t)0x88888888
#define GPIO_CR_AFO_OPENDRAIN          (uint32_t)0xcccccccc 

#define GPIO_CR_AFOUT_PP2MHz          (GPIO_CR_MODE_2MHz | GPIO_CR_AFO_PUSHPULL)  // 复用推挽输出，2MHz
#define GPIO_CR_AFIN_FLOAT            (uint32_t)0x44444444  // 复用开漏输入
#define GPIO_CR_AFIN_PULLDOWN         (uint32_t)0x88888888  // 复用上拉下拉输入


#define USART_CR1_REST                (uint32_t)0x00000000
#define USART_CR2_REST                (uint32_t)0x00000000
#define USART_CR3_REST                (uint32_t)0x00000000



//位带操作,实现51类似的GPIO控制功能
//IO位操作
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
//IO口操作,只对单一的IO口!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 
#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 
#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 
#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 
#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入
#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入
#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入


void Delay_init(uint32_t sysclock);
void Delay(uint32_t nCount);
void Delay_ms(uint16_t nCount);
void Delay_us(uint32_t nCount);


void RCC_init(void);
void NVIC_Config(void);

   
#ifdef __cplusplus
 }
#endif


#endif /* __ARMSIS_H__ */
