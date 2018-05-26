/**
  *	@file       led.h
  * @author     crystalice
  * @version    v1.0
  * @date       2018-4-5
  * @brief      ARM rtc header file
  *
  */
  
#ifndef __LIGHTI_H__
#define __LIGHTI_H__

#include "stm32f1xx.h"

#ifdef __cplusplus
 extern "C" {
#endif


int8_t      Light_init(void);
void        Light_LoadConfig(uint32_t t);
void        Light_Update(uint32_t t);

void        Light_TimeStep(void);


uint8_t     Light_Turn(uint8_t channel);
uint8_t     Light_TurnOf(uint8_t channel, uint8_t state);  
uint8_t     Light_SelectNextChannel(void);
uint8_t     Light_CurrentChannel(void);
void        Light_ChannelTimeSet(uint8_t c, uint16_t ratio, uint32_t t);
   
#define LED_ADJUST_UP       0x0
#define LED_ADJUST_DOWN     0x1
#define LED_ADJUST_TURN     0x2         // 翻转方式
// 调整亮度 channel： Led 通道   state
uint8_t     Light_AdjustLum(uint8_t channel, uint8_t state);

#ifdef __cplusplus
 }
#endif

#endif /* __LIGHTI_H__ */