
// 8路led，引脚对应
//
// time3  TIME 4
//
// channel  color
//    1     Cool White    PA6   ---- LED 1 通道
//    2     Deep Blue     PA7   ---- 2
//    3     Blue          PB0   ---- 3
//    4     Green         PB1   ---- 4
//    5     Photo red     PB6   ---- 5
//    6     UV            PB7   ---- 6 
//    7     Violet        PB8   ---- 7
//    8     Warm white    PB9   ---- 8
//

#include <stddef.h> // needed for definition of NULL
#include <stdlib.h> // malloc。
#include "light.h"

#define MAXCFGS     12      // 可配置亮度比例数量
#define CHANNELCNT  8       // pwm通道数量

typedef struct
{
    uint32_t remnant; // 倒计时状态切换
    uint8_t currrate; // 当前亮度  100%
    uint8_t nextrate; // 下个亮度 （倒计时完成后的亮度) 100%
    uint8_t count;
    uint32_t cfgs[MAXCFGS];  // 100%  31~23位 亮度百分比， 19~0位 时间

} LedItem_t;

#define Light_CfgTime(d)            ((d) & 0xFFFFF)
#define Light_CfgRate(d)            ((d) >> 24 & 0xFF)
#define Light_CfgSetOfTime(r, t)    (((r) & 0xFF) << 24 | ((t) & 0xFFFFF))

typedef struct
{
    uint8_t selectChannel; // 当前选择的通道
    uint8_t channelcount;
    uint8_t maxratio;    // 灯光总最大亮度 100%
    uint16_t updatestate; // 是否需要刷新
    uint32_t switchstate; // 开关状态  00 -- auto   01 -- close   10 -- open
    LedItem_t items[CHANNELCNT];
} LedData_t;

static LedData_t leddata;


//#define time_arr 0x77 // 24(23+1)  12MHz / 120 = 100KHz
#define time_arr  239
#define MaxChannel CHANNELCNT - 1  // 最大led通道

#define Light_LumMaxRatio leddata.maxratio

#define Light_TurnStateGet(c) (leddata.updatestate & (0x1 << (c)))
#define Light_TuruStateSet(c) (leddata.updatestate |= 0x1 << (c))
#define Light_TuruStateClr(c) (leddata.updatestate &= ~(0x1 << (c)))

// 手动开关状态
#define Light_SWITCHSTATE_AUTO  0X00    // 
#define Light_SWITCHSTATE_ON    0x01
#define Light_SWITCHSTATE_OFF   0X10
#define Light_SWITCHSTATE_MARK  0X11

static void Light_LoadChannelData(uint8_t c, uint32_t t);
#define Light_SwitchGet(c)          (leddata.switchstate >> (c << 1) & 0xFF)
#define Light_SwitchSet(c, s)       (leddata.switchstate |= (s) << (c << 1))
#define Light_ChannelTimeStep(c)    if (leddata.items[c].remnant) leddata.items[c].remnant--


#define Light_UpdateChannel(c)      if (Light_TurnStateGet(c)) { Light_TuruStateClr(c); Light_UpdatePWM(c); }
static void Light_UpdatePWM(uint8_t c);
static void Light_TimeRemnantUpdate(uint8_t c, uint32_t t);

void _memset(void *s, uint8_t c, uint32_t count)
{
    uint8_t *d = s;
    while (count--)
        *d++ = c;
}

int8_t Light_init(void)
{
    // 8路led，引脚对应
    //
    // time3
    // PA6   ---- LED 1 通道
    // PA7   ---- 2
    // PB0   ---- 3
    // PB1   ---- 4
    // TIME 4
    // PB6   ---- 5
    // PB7   ---- 6
    // PB8   ---- 7
    // PB9   ---- 8

    // 总线 TIM3 TIM4 和 A,B组引脚 使能
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN | RCC_APB1ENR_TIM4EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN;

    // 输出 2MHz 复用推挽
    // TIME3 PA6 PA7 PB0 PB1
    GPIOA->CRL &= ~(GPIO_CRL_MODE6_Msk | GPIO_CRL_CNF6_Msk); // TIME3 PA6
    GPIOA->CRL |= GPIO_CRL_MODE6_1 | GPIO_CRL_CNF6_1;
    GPIOA->CRL &= ~(GPIO_CRL_MODE7_Msk | GPIO_CRL_CNF7_Msk); // TIME3 PA7
    GPIOA->CRL |= GPIO_CRL_MODE7_1 | GPIO_CRL_CNF7_1;
    GPIOB->CRL &= ~(GPIO_CRL_MODE0_Msk | GPIO_CRL_CNF0_Msk); // TIME3 PB0
    GPIOB->CRL |= GPIO_CRL_MODE0_1 | GPIO_CRL_CNF0_1;
    GPIOB->CRL &= ~(GPIO_CRL_MODE1_Msk | GPIO_CRL_CNF1_Msk); // TIME3 PB1
    GPIOB->CRL |= GPIO_CRL_MODE1_1 | GPIO_CRL_CNF1_1;

    // Time4 2MHz 复用推挽
    GPIOB->CRL &= ~(GPIO_CRL_MODE6_Msk | GPIO_CRL_CNF6_Msk |
                    GPIO_CRL_MODE7_Msk | GPIO_CRL_CNF7_Msk);
    GPIOB->CRH &= ~(GPIO_CRH_MODE8_Msk | GPIO_CRH_CNF8_Msk |
                    GPIO_CRH_MODE9_Msk | GPIO_CRH_CNF9_Msk);
    GPIOB->CRL |= GPIO_CRL_MODE6_1 | GPIO_CRL_CNF6_1 |
                  GPIO_CRL_MODE7_1 | GPIO_CRL_CNF7_1;
    GPIOB->CRH |= GPIO_CRH_MODE8_1 | GPIO_CRH_CNF8_1 |
                  GPIO_CRH_MODE9_1 | GPIO_CRH_CNF9_1;

    //
    // TIME3 配置输出
    //   1、配置前关闭
    //   2、设置比对输出
    //   3、预装载使能  OC模式为PWM1
    //   4、预装载 0.5MHz  = 12000 / (23 + 1) = 500KHz 2u (1MHz = 1u)
    //   5、不分频
    //   6、使能CCER，使能CR
    TIM3->CR1 = 0x0U;
    TIM3->CCER = 0x0U;  // 配置前关闭时钟
    TIM3->CCMR1 = 0x0U; // 清除配置
    TIM3->CCR1 = 0x0;
    TIM3->CCR2 = 0x0;
    TIM3->CCR3 = 0x0;
    TIM3->CCR4 = 0x0;

    TIM3->CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_CC2S);
    TIM3->CCMR2 &= ~(TIM_CCMR2_CC3S | TIM_CCMR2_CC4S);
    TIM3->CCMR1 |= TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 |
                   TIM_CCMR1_OC2PE | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;
    TIM3->CCMR2 |= TIM_CCMR2_OC3PE | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2 |
                   TIM_CCMR2_OC4PE | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2;
    TIM3->ARR = time_arr;
    TIM3->PSC = 0;
    TIM3->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;
    TIM3->CR1 |= TIM_CR1_CEN | TIM_CR1_ARPE;


    // TIME4 设置
    TIM4->CR1 = 0x0U;
    TIM4->CCER = 0X0U;  // 配置前关闭时钟
    TIM4->CCMR1 = 0x0U; // 清除配置
    TIM4->CCR1 = 0x0;
    TIM4->CCR2 = 0x0;
    TIM4->CCR3 = 0x0;
    TIM4->CCR4 = 0x0;

    TIM4->CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_CC2S);
    TIM4->CCMR2 &= ~(TIM_CCMR2_CC3S | TIM_CCMR2_CC4S);
    TIM4->CCMR1 |= TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 |
                   TIM_CCMR1_OC2PE | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;
    TIM4->CCMR2 |= TIM_CCMR2_OC3PE | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2 |
                   TIM_CCMR2_OC4PE | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2;
    TIM4->ARR = time_arr;
    TIM4->PSC = 0;
    TIM4->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;
    TIM4->CR1 |= TIM_CR1_CEN | TIM_CR1_ARPE;

    return 0;
}

static void Light_LoadChannelData(uint8_t c, uint32_t t)
{
    uint8_t i;
    if (leddata.items[c].count == 0) {
        return ;
    }
       
    i = leddata.items[c].count - 1;
    while (i > 0 && (Light_CfgTime(leddata.items[c].cfgs[i]) > t)) i--;

    leddata.items[c].remnant = 0;
    leddata.items[c].nextrate = 0; 
    if (t >= Light_CfgTime(leddata.items[c].cfgs[i])) {
        leddata.items[c].nextrate = Light_CfgRate(leddata.items[c].cfgs[i]);
    }
    
    
    Light_TimeRemnantUpdate(c, t);
}

static void Light_AddChannelCfg(uint8_t c, uint16_t r, uint16_t h, uint16_t m, uint16_t s) {
    // 设置通道时间
    uint32_t t;

    t = h * 60 * 60 + m * 60 + s;
    if (leddata.items[c].count >= MAXCFGS) return;
    leddata.items[c].cfgs[leddata.items[c].count] = Light_CfgSetOfTime(r, t);
    leddata.items[c].count++;
}

void Light_LoadConfig()
{
    // channel  color
    //    1     Cool White    PA6   ---- LED 1 通道
    //    2     Deep Blue     PA7   ---- 2
    //    3     Blue          PB0   ---- 3
    //    4     Green         PB1   ---- 4
    //    5     Photo red     PB6   ---- 5
    //    6     UV            PB7   ---- 6 
    //    7     Violet        PB8   ---- 7
    //    8     Warm white    PB9   ---- 8
    leddata.maxratio = 100;
    leddata.channelcount = 8;
    leddata.selectChannel = 0;

    // 1     Cool White
    Light_AddChannelCfg(0, 10, 8, 0, 0);
    Light_AddChannelCfg(0, 30, 8, 10, 0);
    Light_AddChannelCfg(0, 60, 8, 30, 0);
    Light_AddChannelCfg(0, 90, 9, 0, 0);
    Light_AddChannelCfg(0, 100, 10, 0, 0);
    Light_AddChannelCfg(0, 90, 15, 0, 0);
    Light_AddChannelCfg(0, 80, 16, 0, 0);
    Light_AddChannelCfg(0, 50, 16, 30, 0);
    Light_AddChannelCfg(0, 20, 17, 0, 0);
    Light_AddChannelCfg(0, 0, 18, 0, 0);

    //    2     Deep Blue
    Light_AddChannelCfg(1, 10, 9, 0, 0);
    Light_AddChannelCfg(1, 50, 9, 30, 0);
    Light_AddChannelCfg(1, 80, 10, 0, 0);
    Light_AddChannelCfg(1, 100, 11, 0, 0);
    Light_AddChannelCfg(1, 90, 14, 30, 0);
    Light_AddChannelCfg(1, 80, 15, 0, 0);
    Light_AddChannelCfg(1, 50, 15, 30, 0);
    Light_AddChannelCfg(1, 10, 16, 0, 0);
    Light_AddChannelCfg(1, 0, 16, 30, 0);
    //    3     Blue
    Light_AddChannelCfg(2, 10, 9, 0, 0);
    Light_AddChannelCfg(2, 50, 9, 30, 0);
    Light_AddChannelCfg(2, 80, 10, 0, 0);
    Light_AddChannelCfg(2, 100, 11, 0, 0);
    Light_AddChannelCfg(2, 90, 14, 30, 0);
    Light_AddChannelCfg(2, 80, 15, 0, 0);
    Light_AddChannelCfg(2, 50, 15, 30, 0);
    Light_AddChannelCfg(2, 10, 16, 0, 0);
    Light_AddChannelCfg(2, 0, 16, 30, 0);
    //    4     Green
    Light_AddChannelCfg(3, 10, 11, 0, 0);
    Light_AddChannelCfg(3, 80, 12, 0, 0);
    Light_AddChannelCfg(3, 100, 13, 0, 0);
    Light_AddChannelCfg(3, 80, 14, 0, 0);
    Light_AddChannelCfg(3, 0, 15, 0, 0);
    //    5     Photo red
    Light_AddChannelCfg(4, 10, 11, 0, 0);
    Light_AddChannelCfg(4, 80, 12, 0, 0);
    Light_AddChannelCfg(4, 100, 13, 0, 0);
    Light_AddChannelCfg(4, 80, 14, 0, 0);
    Light_AddChannelCfg(4, 0, 15, 0, 0);
    //    6     UV
    Light_AddChannelCfg(5, 10, 11, 0, 0);
    Light_AddChannelCfg(5, 80, 12, 0, 0);
    Light_AddChannelCfg(5, 100, 13, 0, 0);
    Light_AddChannelCfg(5, 80, 14, 0, 0);
    Light_AddChannelCfg(5, 0, 15, 0, 0);
    //    7     Violet
    Light_AddChannelCfg(6, 10, 11, 0, 0);
    Light_AddChannelCfg(6, 80, 12, 0, 0);
    Light_AddChannelCfg(6, 100, 13, 0, 0);
    Light_AddChannelCfg(6, 80, 14, 0, 0);
    Light_AddChannelCfg(6, 0, 15, 0, 0);
    //    8     Warm white
    Light_AddChannelCfg(7, 30,  8,  30, 0);
    Light_AddChannelCfg(7, 60,  8,  50, 0);
    Light_AddChannelCfg(7, 90,  9,  10, 0);
    Light_AddChannelCfg(7, 100, 10,  0, 0);
    Light_AddChannelCfg(7, 90,  15,  0, 0);
    Light_AddChannelCfg(7, 80,  16,  0, 0);
    Light_AddChannelCfg(7, 50,  16, 30, 0);
    Light_AddChannelCfg(7, 20,  17,  0, 0);
    Light_AddChannelCfg(7, 0,   18, 30, 0);
}

void Light_InitChannelTimes(uint32_t t)
{
    uint8_t c;
    for (c = 0; c < leddata.channelcount; c++) {
        Light_LoadChannelData(c, t);
    }
}


void Light_Update(uint32_t t)
{
    uint8_t c;
    for (c = 0; c < leddata.channelcount; c++)
    {
        if (!leddata.items[0].remnant) {
            Light_TimeRemnantUpdate(0, t);
        }
        
        if (Light_TurnStateGet(c)) { 
            Light_TuruStateClr(c); 
            Light_UpdatePWM(c); 
        } 
    }
}

static void Light_UpdatePWM(uint8_t c)
{
    // 更新PWM分频值
    uint32_t pwmwidth;

    if (Light_SwitchGet(c) & Light_SWITCHSTATE_ON) 
        pwmwidth = time_arr * Light_LumMaxRatio / 100;
    else if (Light_SwitchGet(c) & Light_SWITCHSTATE_OFF)
        pwmwidth = 0;
    else
    {
        // 分频值计算
        //   PWM            time_arr （20u 50KHz）
        //   最大亮度       Light_LumMaxRatio
        //
        //   Time最大计数  time_arr * 最大亮度%
        //   当前比率值 = time_arr * 最大亮度% * 当前比率%
        //
        //   由于分频值比较小，优先所有值先乘后除
        pwmwidth = time_arr * Light_LumMaxRatio * leddata.items[c].currrate / 10000;
    }

    switch (c) {
    case 0: TIM3->CCR1 = pwmwidth; break;
    case 1: TIM3->CCR2 = pwmwidth; break;
    case 2: TIM3->CCR3 = pwmwidth; break;
    case 3: TIM3->CCR4 = pwmwidth; break;
    case 4: TIM4->CCR1 = pwmwidth; break;
    case 5: TIM4->CCR2 = pwmwidth; break;
    case 6: TIM4->CCR3 = pwmwidth; break;
    case 7: TIM4->CCR4 = pwmwidth; break;
    }
}

static void Light_TimeRemnantUpdate(uint8_t c, uint32_t t)
{
    uint8_t i;
    
    i = 0;
    while (i < leddata.items[c].count && (Light_CfgTime(leddata.items[c].cfgs[i]) < t)) i++;

    if (i < leddata.items[c].count)
        leddata.items[c].remnant = Light_CfgTime(leddata.items[c].cfgs[i]) - t;
    else
        leddata.items[c].remnant = Light_CfgTime(leddata.items[c].cfgs[i]) + (24 * 60 * 60 - t);

    if (i >= leddata.items[c].count) i = 0;

    leddata.items[c].currrate = leddata.items[c].nextrate;
    leddata.items[c].nextrate = Light_CfgRate(leddata.items[c].cfgs[i]);
    Light_TuruStateSet(c);
}

void Light_ChannelTimeSet(uint8_t c, uint16_t ratio, uint32_t t)
{
    // 设置通道时间

    t *= 60; // 把分进位成秒

    if (leddata.items[c].count >= MAXCFGS) return;

    leddata.items[c].cfgs[leddata.items[c].count] = Light_CfgSetOfTime(ratio, t);
    leddata.items[c].count++;
}

uint8_t Light_AdjustLum(uint8_t channel, uint8_t state)
{
    // 调节通道名亮度
    return 0;
}

void Light_TimeStep(void)
{
    uint8_t c;
    for (c = 0; c < leddata.channelcount; c++) {   
        Light_ChannelTimeStep(c);
    }
}

uint8_t Light_Turn(uint8_t c)
{
    uint8_t s;

    s = Light_SwitchGet(c);
    if (s == Light_SWITCHSTATE_AUTO) 
        s = Light_SWITCHSTATE_ON;
    else if (s == Light_SWITCHSTATE_ON)
        s = Light_SWITCHSTATE_OFF;
    else
        s = Light_SWITCHSTATE_AUTO;
    
    leddata.switchstate &= ~(Light_SWITCHSTATE_MARK << (c << 1));
    leddata.switchstate |= s << (c << 1);
    
    Light_SwitchSet(c, s);
    Light_UpdatePWM(c);

    return Light_SwitchGet(c);
}

uint8_t Light_SelectNextChannel(void)
{
    leddata.selectChannel++;
    if (leddata.selectChannel >= leddata.channelcount)
        leddata.selectChannel = 0;
    return leddata.selectChannel;
}

uint8_t Light_CurrentChannel(void)
{
    return leddata.selectChannel;
}

uint8_t Light_TurnOf(uint8_t c, uint8_t state)
{
    // 控制单个
    Light_TuruStateSet(c);
    return 0;
}
