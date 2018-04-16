/**
  *	@file       led.h
  * @author     crystalice
  * @version    v1.0
  * @date       2018-4-5
  * @brief      ARM rtc header file
  *
  */
  
#ifndef __LED_H__
#define __LED_H__

#include "stm32f1xx.h"

#ifdef __cplusplus
 extern "C" {
#endif


int8_t      Led_init(void);
void        Led_Update(void);

int8_t      Led_TurnOf(uint8_t id, uint8_t state);   

   
#ifdef __cplusplus
 }
#endif


#endif /* __LED_H__ */
