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
   

#include "stm32f10x.h"
#include "serial.h"

   
/*******************  definition for GPIO_CR register  *******************/
#define GPIO_CR_RESET                 (uint32_t)0x44444444 
   
#define GPIO_CRL_SET0                 (GPIO_CRL_MODE0  | GPIO_CRL_CNF0)
#define GPIO_CRL_SET1                 (GPIO_CRL_MODE1  | GPIO_CRL_CNF1)
#define GPIO_CRL_SET2                 (GPIO_CRL_MODE2  | GPIO_CRL_CNF2)
#define GPIO_CRL_SET3                 (GPIO_CRL_MODE3  | GPIO_CRL_CNF3)
#define GPIO_CRL_SET4                 (GPIO_CRL_MODE4  | GPIO_CRL_CNF4)
#define GPIO_CRL_SET5                 (GPIO_CRL_MODE5  | GPIO_CRL_CNF5)
#define GPIO_CRL_SET6                 (GPIO_CRL_MODE6  | GPIO_CRL_CNF6)
#define GPIO_CRL_SET7                 (GPIO_CRL_MODE7  | GPIO_CRL_CNF7)
#define GPIO_CRH_SET8                 (GPIO_CRH_MODE8  | GPIO_CRH_CNF8)
#define GPIO_CRH_SET9                 (GPIO_CRH_MODE9  | GPIO_CRH_CNF9)
#define GPIO_CRH_SET10                (GPIO_CRH_MODE10 | GPIO_CRH_CNF10)
#define GPIO_CRH_SET11                (GPIO_CRH_MODE11 | GPIO_CRH_CNF11)
#define GPIO_CRH_SET12                (GPIO_CRH_MODE12 | GPIO_CRH_CNF12)
#define GPIO_CRH_SET13                (GPIO_CRH_MODE13 | GPIO_CRH_CNF13)
#define GPIO_CRH_SET14                (GPIO_CRH_MODE14 | GPIO_CRH_CNF14)
#define GPIO_CRH_SET15                (GPIO_CRH_MODE15 | GPIO_CRH_CNF15)
   
#define GPIO_CR_MODE_INPUT            (uint32_t)0x00000000
#define GPIO_CR_MODE_2MHz             (uint32_t)0x22222222
#define GPIO_CR_MODE_10MHz            (uint32_t)0x11111111
#define GPIO_CR_MODE_50MHz            (uint32_t)0x33333333
   
#define GPIO_CR_ICNF_ANALOG           (uint32_t)0x00000000  
#define GPIO_CR_ICNF_FLOAT            (uint32_t)0x44444444  // 开漏
#define GPIO_CR_ICNF_UPDOWN           (uint32_t)0x88888888  
   
#define GPIO_CR_OCNF_GPPP             (uint32_t)0x00000000  
#define GPIO_CR_OCNF_GPOD             (uint32_t)0x44444444  // 开漏
#define GPIO_CR_OCNF_AFPP             (uint32_t)0x88888888  
#define GPIO_CR_OCNF_AFOD             (uint32_t)0xCCCCCCCC  

#define GPIO_CR_GPOUT_PP2MHz          (GPIO_CR_MODE_2MHz  | GPIO_CR_OCNF_GPPP)  // 通用推挽 2MHz
#define GPIO_CR_GPOUT_PP50MHz         (GPIO_CR_MODE_50MHz | GPIO_CR_OCNF_GPPP)  // 通用推挽 50MHz
#define GPIO_CR_AFOUT_PP2MHz          (GPIO_CR_MODE_2MHz  | GPIO_CR_OCNF_AFPP)  // 复用推挽 2MHz
#define GPIO_CR_AFOUT_PP50MHz         (GPIO_CR_MODE_50MHz | GPIO_CR_OCNF_AFPP)  // 复用推挽 2MHz

#define GPIO_CR_IN_FLOAT              (GPIO_CR_MODE_INPUT | GPIO_CR_ICNF_FLOAT)
#define GPIO_CR_IN_UPDOWN             (GPIO_CR_MODE_INPUT | GPIO_CR_ICNF_UPDOWN)


/*******************  definition for USART CR register  *******************/
#define USART_CR1_REST                (uint32_t)0x00000000
#define USART_CR2_REST                (uint32_t)0x00000000
#define USART_CR3_REST                (uint32_t)0x00000000

   
void Delay_init(void);
void Delay(uint32_t nCount);
void Delay_ms(uint16_t nCount);
void Delay_us(uint32_t nCount);

   
#ifdef __cplusplus
 }
#endif


#endif /* __ARMSIS_H__ */
