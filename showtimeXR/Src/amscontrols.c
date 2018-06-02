#include "light.h"


static uint32_t Key_StateValues = 0x0;



#define KEY_SELECT                  0x1U << 0U
#define KEY_TURN                    0x1U << 1U
#define KEY_PWM                     0x1U << 2U
#define KEY_TIME                    0x1U << 3U


#define Key_Clicked(keyid)          (Key_StateValues & (keyid))
#define Key_StateClr(keyid)         (Key_StateValues &= ~(keyid))
#define Key_StateSet(keyid)         (Key_StateValues |= (keyid))



void AMSKey_Init(void)
{
    // 
    // 触摸板
    //   初态为低电平，触摸为高电平，不触摸为低电平
    // 
    // PB12 mini板上连了LED
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; 
    
    GPIOB->CRH &= ~(GPIO_CRH_MODE13_Msk | GPIO_CRH_CNF13_Msk);
    GPIOB->CRH |= GPIO_CRH_CNF13_1;// MODE13_0; 
}

void AMSLedState_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; 
    
    GPIOB->CRH &= ~(GPIO_CRH_MODE12_Msk | GPIO_CRH_CNF12_Msk);
    GPIOB->CRH |= GPIO_CRH_MODE12_1; // 推挽输出 2M
}

void AMSKey_Check(void)
{
    if (WK_Trun == 1) 
    {
        
        //LED_STATE = 1;
        //Delay_ms(10); // 去抖
        if(WK_Trun == 1) {
            //LED_STATE = ~LED_STATE;
            //Key_StateSet(KEY_TURN);
            Key_StateValues |= 1 << 5U;
        }
    }
    else if (Key_StateValues & (1 << 5)) {
        Key_StateValues &= ~(1 << 5);
        Key_StateSet(KEY_TURN);
    }
}

void Key_Update(void) {
    AMSKey_Check();
  
    if (Key_Clicked(KEY_SELECT)) {
        Key_StateClr(KEY_SELECT);
        Light_SelectNextChannel();
    }
    else if (Key_Clicked(KEY_TURN)) {
        Key_StateClr(KEY_TURN);
        Light_Turn(Light_CurrentChannel());
        
    }
    else if (Key_Clicked(KEY_PWM)) {
        Key_StateClr(KEY_PWM);
        Light_AdjustLum(Light_CurrentChannel(), LED_ADJUST_TURN);
    }
    
}
