/**
  *	@file       rtc.h
  * @author     crystalice
  * @version    v1.0
  * @date       2018-4-5
  * @brief      ARM rtc header file
  *
  */
  
#ifndef __RTC_H__
#define __RTC_H__

#include "stm32f1xx.h"

#ifdef __cplusplus
 extern "C" {
#endif
   
#define TIMEKIND_HOUR   4
#define TIMEKIND_MIN    5

uint8_t RTC_init(void);
uint8_t RTC_initOther(void);
uint8_t RTC_Set(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec);
uint32_t RTC_GetTime(void);
uint32_t RTC_UpTimeOf(uint8_t kind); // kind = 1
   
#ifdef __cplusplus
 }
#endif


#endif /* __RTC_H__ */
