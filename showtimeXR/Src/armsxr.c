
#include "stm32f1xx.h"
#include "armsis.h"
#include "rtc.h"
#include "light.h"


//#define SystemCoreClock    SYSCLK_FREQ_72MHz
uint32_t SystemCoreClock = SYSCLK_FREQ_48MHz;
uint32_t currentTime     = 0;
uint8_t  secFlag         = 0;

void SystemInit(void)
{
    // 系统时钟、总线和GPIO状态
    // 注：初始化只能设置寄存器状态，不能初始自定义变量
    RCC_init();
    RTC_init();
    Light_init();
}

void amsXR_LoadConfig(void)
{
    Delay_init(SystemCoreClock);

    RTC_Set(2018, 4, 16, 7, 59, 59);
    currentTime = RTC_GetTime();
    Light_LoadConfig(currentTime);
    NVIC_Config();
}

int main(void)
{
    amsXR_LoadConfig();

    while (1) {
        if (secFlag) {
            Light_TimeStep();
            Light_Update(currentTime);
            secFlag = 0;
        }
        Key_Update();
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

