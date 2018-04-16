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
void        Led_LoadConfig(void);
void        Led_Update(uint32_t t);

void        Led_TimeStep(void);


uint8_t     Led_Turn(uint8_t channel);
uint8_t     Led_TurnOf(uint8_t channel, uint8_t state);  
uint8_t     Led_SelectNextChannel(void);
uint8_t     Led_CurrentChannel(void);
void        Led_ChannelTimeSet(uint8_t c, uint16_t lum, uint32_t t);
   
#define LED_ADJUST_UP       0x0
#define LED_ADJUST_DOWN     0x1
#define LED_ADJUST_TURN     0x2         // 翻转方式
// 调整亮度 channel： Led 通道   state
uint8_t     Led_AdjustLum(uint8_t channel, uint8_t state);

#ifdef __cplusplus
 }
#endif

#endif /* __LED_H__ */
