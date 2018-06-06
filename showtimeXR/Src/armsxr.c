
#include "stm32f1xx.h"
#include "armsis.h"
#include "rtc.h"
#include "light.h"
#include "oled.h"
#include "screenbk.h"


//#define SystemCoreClock    SYSCLK_FREQ_72MHz
uint32_t SystemCoreClock = SYSCLK_FREQ_48MHz;
uint8_t  secFlag         = 0;
uint32_t currentTime     = 0;
uint32_t Light_States    = 0x0;
 

void SystemInit(void)
{
    // 系统时钟、总线和GPIO状态
    // 注：初始化只能设置寄存器状态，不能初始自定义变量
    RCC_init();
    RTC_init();
    Light_init();
    AMSKey_Init();
    AMSLedState_init();
}

void amsXR_LoadConfig(void)
{
    Delay_init(SystemCoreClock);

    RTC_Set(2018, 4, 16, 7, 0, 0);
    currentTime = RTC_GetTime();
    Light_LoadConfig();
    NVIC_Config();
    
}

int main(void)
{
    Delay_init(SystemCoreClock);
    OLED_Init();
    
    amsXR_LoadConfig();
    Light_Update(currentTime);
    
    OLED_Clear();
    OLED_DrawBMP(0, 0, 128, 4, gImage_screenbk);
    //OLED_ShowString(0, 0, "0123456789", 8); //DrawBMP
    AMSScreen_ShowTime(currentTime);
    Light_InitChannelTimes(currentTime);
    
    while (1) {  
       if (Key_Update()) {
           currentTime = RTC_GetTime();
           AMSScreen_ShowTime(currentTime);
           Light_InitChannelTimes(currentTime);
       }
        
        if (secFlag) {
            AMSScreen_ShowTime(currentTime);
            Light_TimeStep();
            Light_Update(currentTime);
            secFlag = 0;
        }

    }
}

void RTC_IRQHandler(void)
{
    if (RTC->CRL & RTC_CRL_SECF) {        
        currentTime = RTC_GetTime();
        RTC->CRL &= ~RTC_CRL_SECF;
        while (!(RTC->CRL & RTC_CRL_RTOFF)) __nop();
        secFlag++;
    }
}

