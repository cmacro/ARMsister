
#include "stm32f1xx.h"
#include "armsis.h"
#include "rtc.h"
#include "light.h"



#define GPIO_CRH_MODE9_2MHz   GPIO_CRH_MODE9_1
#define GPIO_CRH_CNF9_AOPP    GPIO_CRH_CNF9_1
#define TIM_CR2_MMS_CP        (TIM_CR2_MMS_0 | TIM_CR2_MMS_1)
#define TIM_CCMR2_OC4M_PWM1   (TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2)


#define LED0 PAout(8) // PA8
#define LED1 PBout(2) // PB2

uint32_t Key_StateValues = 0x0;
uint32_t currentTime     = 0;
uint8_t  secFlag         = 0;


void _NVIC_PriorityGroupConfig(uint8_t group)
{
    uint32_t temp, temp1;
    temp1 = (~group) & 0x07;//取后三位
    temp1 <<= 8;
    temp = SCB->AIRCR;      //读取先前的设置
    temp &= 0X0000F8FF;     //清空先前分组
    temp |= 0X05FA0000;     //写入钥匙
    temp |= temp1;
    SCB->AIRCR = temp;          //设置分组
}

void _NVIC_Init(uint8_t preemption, uint8_t sub, uint8_t channel,
                uint8_t group)
{
    uint32_t temp;
    _NVIC_PriorityGroupConfig(group);    //设置分组
    temp = preemption << (4 - group);
    temp |= sub & (0x0f >> group);
    temp &= 0xf;                        //取低四位
    NVIC->ISER[channel / 32] |= (1 << channel % 32);
    //使能中断位(要清除的话,相反操作就 OK)
    NVIC->IP[channel] |= temp << 4;//设置响应优先级和抢断优先级
}

void RCC_init(void)
{
    // 使用外部8M晶振，启用PPL，系统时钟为48MHz，总线时钟12HMz
    RCC->CFGR = RCC_CFGR_SW_PLL |       // 启用PPL
                RCC_CFGR_PLLSRC |       // 使用PPL作为时钟源
                RCC_CFGR_PLLMULL6 |     // 6倍频，系统时钟 48Mhz
                RCC_CFGR_HPRE_DIV4 |    // 预分频，总线时钟 12MHz
                RCC_CFGR_PPRE1_DIV1 |   // 低速总线 12MHz
                RCC_CFGR_PPRE2_DIV1 |   // 高速总线 12MHz
                RCC_CFGR_USBPRE;        // USB直接使用PPL倍频时钟 48MHz

    // 开启PPL和外部时钟
    RCC->CR =   RCC_CR_PLLON |
                RCC_CR_HSEON;

    // 闪存访问延迟，48MHz ~ 72MHz = 010。 复位值：0x30
    // 01 Flash访问周期 (24Mhz ~ 48MHz] 范围
    // AHB时钟分频 != 1 时必须开启
    FLASH->ACR = FLASH_ACR_LATENCY_0 |
                 FLASH_ACR_PRFTBE |
                 FLASH_ACR_PRFTBS;


    // 确定外部高速晶振起效
    while (!(RCC->CR & RCC_CR_HSERDY));
    // 确定PLL设置起效
    while (!(RCC->CR & RCC_CR_PLLRDY));
    // 确定PLL为系统时钟源
    while (!(RCC->CFGR & RCC_CFGR_SWS_PLL));
}

void SystemInit(void)
{
    RCC_init();
    RTC_init();
    Light_init();
}

#define KEY_SELECT                  0x1
#define KEY_TURN                    0x2
#define KEY_PWM                     0x4
#define KEY_TIME                    0x8


#define Key_Clicked(keyid)          (Key_StateValues & keyid)
#define Key_StateClr(keyid)         (Key_StateValues &= ~keyid)
#define Key_StateSet(keyid)         (Key_StateValues |= keyid)


void Key_Update(void) {
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

void NVIC_Config(void) {
    _NVIC_Init(0, 0, RTC_IRQn, 2);
}

void _Delay(unsigned int v)
{
    unsigned int x, y, iDelay;
    for (iDelay = 0; iDelay < v; iDelay++)
        for (x = 0; x < 100; x++)
            for (y = 0; y < 1000; y++)
                __nop();
}

void AM_InitLed()
{

    RCC->APB2ENR |= 1 << 2; //使能 PORTA 时钟
    RCC->APB2ENR |= 1 << 3; //使能 PORTB 时钟
    GPIOA->CRH &= 0XFFFFFFF0;
    GPIOA->CRH |= 0X00000003; //PA8 推挽输出
    GPIOA->ODR |= 1 << 8; //PA8 输出高
    GPIOB->CRL &= 0XFFFFF0FF;
    GPIOB->CRL |= 0X00000300; //PD.2 推挽输出
    GPIOB->ODR |= 1 << 2; //PD.2 输出高

    GPIOC->CRH &= 0XFFF00FFF;
    GPIOC->CRH |= 0X00038000;
    GPIOC->ODR = 1 << 11;
    GPIOC->ODR ^= 1 << 12;
}

void amsXR_LoadConfig(void)
{
    Delay_init();

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

