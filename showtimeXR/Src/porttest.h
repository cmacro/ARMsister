#ifndef __PORTTEST_H__
#define __PORTTEST_H__

#include "stm32f1xx.h"

#define GPIO_CRH_MODE9_2MHz   GPIO_CRH_MODE9_1
#define GPIO_CRH_CNF9_AOPP    GPIO_CRH_CNF9_1
#define TIM_CR2_MMS_CP        (TIM_CR2_MMS_0 | TIM_CR2_MMS_1)
#define TIM_CCMR2_OC4M_PWM1   (TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2)


#define LED0 PAout(8) // PA8
#define LED1 PBout(2) // PB2


void AM_InitTestLed(void);

#endif /* __PORTTEST_H__ */
