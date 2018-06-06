#include "light.h"
#include "rtc.h"


static uint32_t Key_StateValues = 0x0;


#define WK_Trun     PBin(13)        // 开关键
#define WK_Set      PBin(14)        // 设置开关

#define KEY_SELECT                  (0x1U << 0U)
#define KEY_TURN                    (0x1U << 1U)
#define KEY_PWM                     (0x1U << 2U)
#define KEY_TIME                    (0x1U << 3U)

#define KEY_TIMEHOUR                (0x1U << 8U)
#define KEY_TIMEMIN                 (0x1U << 9U)


#define Key_Clicked(keyid)          (Key_StateValues & (keyid))
#define Key_StateClr(keyid)         (Key_StateValues &= ~(keyid))
#define Key_StateSet(keyid)         (Key_StateValues |= (keyid))

#define KeyClickStateDown()         (LED_STATE = 0)
#define KeyClickStateUp()           (LED_STATE = 1)

void AMSKey_Init(void)
{
    // 
    // 触摸板
    //   初态为低电平，触摸为高电平，不触摸为低电平
    // 
    // PB12 mini板上连了LED
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; 
    
    GPIOB->CRH &= ~(GPIO_CRH_MODE13_Msk | GPIO_CRH_CNF13_Msk |
                    GPIO_CRH_MODE14_Msk | GPIO_CRH_CNF14_Msk);
    GPIOB->CRH |= GPIO_CRH_CNF13_1 | GPIO_CRH_CNF14_1;// MODE13_0; 
}

void AMSLedState_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; 
    
    GPIOB->CRH &= ~(GPIO_CRH_MODE12_Msk | GPIO_CRH_CNF12_Msk);
    GPIOB->CRH |= GPIO_CRH_MODE12_1; // 推挽输出 2M
    
    LED_STATE = 1;
}

uint8_t AMSKey_Check(void)
{
    uint8_t s = 0;
    //LED_STATE = WK_Trun != 1;
    if (WK_Set == 1) {
        Delay_ms(10); // 去抖
        if (WK_Set == 1) {
            Key_StateSet(KEY_TIME);
            KeyClickStateDown();
        }
    }
    else if (WK_Trun == 1) {
        
        Delay_ms(10); // 去抖
        if(WK_Trun == 1) {
            Key_StateSet(KEY_TURN);
            KeyClickStateDown();
        }
    } 
    else {
        s = 1;
        if (Key_Clicked(KEY_TIME)) {
            Key_StateClr(KEY_TIME);
            if (Light_StateClick(LIGHTSTATE_SETHOUR)) {
                Light_StateSet(LIGHTSTATE_SETMIN);
                Light_StateClr(LIGHTSTATE_SETHOUR);
            }
            else if (Light_StateClick(LIGHTSTATE_SETMIN)) {
                Light_StateClr(LIGHTSTATE_SETMIN);
            }
            else {
                Light_StateSet(LIGHTSTATE_SETHOUR);
            }
        }
        else if (Key_Clicked(KEY_TURN)) {
            Key_StateClr(KEY_TURN);
            if (Light_StateClick(LIGHTSTATE_SETHOUR)) {
                RTC_UpTimeOf(TIMEKIND_HOUR);
            } 
            else if (Light_StateClick(LIGHTSTATE_SETMIN)) {
                RTC_UpTimeOf(TIMEKIND_MIN);    
            }
            
        }
        else{
            s = 0;
        }            
            
        KeyClickStateUp();
    }
    
    return s;
}

uint8_t Key_Update(void) {
    return AMSKey_Check();
  
    /*
    if (Key_Clicked(KEY_SELECT)) {
        Key_StateClr(KEY_SELECT);
        Light_SelectNextChannel();
    }
    else if (Key_Clicked(KEY_TURN)) {
        Key_StateClr(KEY_TURN);
        //Light_Turn(Light_CurrentChannel());
        
    }
    else if (Key_Clicked(KEY_PWM)) {
        Key_StateClr(KEY_PWM);
        Light_AdjustLum(Light_CurrentChannel(), LED_ADJUST_TURN);
    }
    */
    
}
